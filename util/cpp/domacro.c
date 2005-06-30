/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* PREPROCESSOR: CONTROLLINE INTERPRETER */

#include	"interface.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"debug.h"
#include	"idf.h"
#include	"input.h"

#include	"ifdepth.h"	
#include	"botch_free.h"	
#include	"nparams.h"	
#include	"parbufsize.h"	
#include	"textsize.h"	
#include	"idfsize.h"	
#include	<assert.h>
#include	<alloc.h>
#include	"class.h"
#include	"macro.h"
#include	"bits.h"

IMPORT char **inctable;		/* list of include directories		*/
IMPORT char *getwdir();
PRIVATE char ifstack[IFDEPTH];	/* if-stack: the content of an entry is	*/
				/* 1 if a corresponding ELSE has been	*/
				/* encountered.				*/

int nestlevel = -1;
int svnestlevel[30] = {-1};
int nestcount;
extern int do_preprocess;

char *
GetIdentifier()
{
	/*	returns a pointer to the descriptor of the identifier that is
		read from the input stream. A null-pointer is returned if
		the input does not contain an identifier.
		The substitution of macros is disabled.
	*/
	int tok;
	struct token tk;

	ReplaceMacros = 0;
	tok = GetToken(&tk);
	ReplaceMacros = 1;
	return tok == IDENTIFIER ? tk.tk_str : (char *)0;
}

/*	domacro() is the control line interpreter. The '#' has already
	been read by the lexical analyzer by which domacro() is called.
	The token appearing directly after the '#' is obtained by calling
	the basic lexical analyzing function GetToken() and is interpreted
	to perform the action belonging to that token.
	An error message is produced when the token is not recognized,
	i.e. it is not one of "define" .. "undef" , integer or newline.
*/
EXPORT
domacro()
{
	struct token tk;	/* the token itself			*/
	register struct idf *id;

	switch(GetToken(&tk)) {		/* select control line action	*/
	case IDENTIFIER:		/* is it a macro keyword?	*/
		id = findidf(tk.tk_str);
		if (!id) {
			error("%s: unknown control", tk.tk_str);
			skipline();
			free(tk.tk_str);
			break;
		}
		free(tk.tk_str);
		switch (id->id_resmac) {
		case K_DEFINE:				/* "define"	*/
			do_define();
			break;
		case K_ELIF:				/* "elif"	*/
			do_elif();
			break;
		case K_ELSE:				/* "else"	*/
			do_else();
			break;
		case K_ENDIF:				/* "endif"	*/
			do_endif();
			break;
		case K_IF:				/* "if"		*/
			do_if();
			break;
		case K_IFDEF:				/* "ifdef"	*/
			do_ifdef(1);
			break;
		case K_IFNDEF:				/* "ifndef"	*/
			do_ifdef(0);
			break;
		case K_INCLUDE:				/* "include"	*/
			do_include();
			break;
		case K_LINE:				/* "line"	*/
			/*	set LineNumber and FileName according to
				the arguments.
			*/
			if (GetToken(&tk) != INTEGER) {
				error("#line without linenumber");
				PushBack();
				skipline();
			}
			else
				do_line((unsigned int)tk.tk_val);
			break;
		case K_UNDEF:				/* "undef"	*/
			do_undef();
			break;
		case K_PRAGMA:				/* "pragma"	*/
			/*	ignore for now
			*/
			skipline();
			break;
		default:
			/* invalid word seen after the '#'	*/
			error("%s: unknown control", id->id_text);
			skipline();
		}
		break;
	case INTEGER:		/* # <integer> [<filespecifier>]?	*/
		do_line((unsigned int)tk.tk_val);
		break;
	case EOF:	/* only `#' on this line: do nothing, ignore	*/
		break;
	default:	/* invalid token following '#'		*/
		error("illegal # line");
		PushBack();
		skipline();
	}
}

PRIVATE
skip_block(to_endif)
{
	/*	skip_block() skips the input from
		1)	a false #if, #ifdef, #ifndef or #elif until the
			corresponding #elif (resulting in true), #else or
			#endif is read.
		2)	a #else corresponding to a true #if, #ifdef,
			#ifndef or #elif until the corresponding #endif is
			seen.
	*/
	register int ch;
	register int skiplevel = nestlevel; /* current nesting level	*/
	struct token tk;
	struct idf *id;

	NoUnstack++;
	for (;;) {
		LoadChar(ch);	/* read first character after newline	*/
		if (ch != '#') {
			if (ch == EOI) {
				NoUnstack--;
				return;
			}
			PushBack();
			skipline();
			continue;
		}
		if (GetToken(&tk) != IDENTIFIER) {
			PushBack();
			skipline();
			continue;
		}
		/*	an IDENTIFIER: look for #if, #ifdef and #ifndef
			without interpreting them.
			Interpret #else, #elif and #endif if they occur
			on the same level.
		*/
		id = findidf(tk.tk_str);
		free(tk.tk_str);
		if (id) switch(id->id_resmac) {
		default:
			skipline();
			break;
		case K_IF:
		case K_IFDEF:
		case K_IFNDEF:
			push_if();
			skipline();
			continue;
		case K_ELIF:
			if (ifstack[nestlevel])
				warning("#elif after #else/#elif");
			if (! to_endif && nestlevel == skiplevel) {
				nestlevel--;
				push_if();
				if (ifexpr()) {	/* implicit skipline() */
					NoUnstack--;
					return;
				}
			}
			else skipline();
			break;
		case K_ELSE:
			if (ifstack[nestlevel])
				warning("#else after #else/#elif");
			skipline();
			if (! to_endif) {
				++(ifstack[nestlevel]);
				if (nestlevel == skiplevel) {
					NoUnstack--;
					return;
				}
			}
			break;
		case K_ENDIF:
			assert(nestlevel >= 0);
			skipline();
			if (nestlevel == skiplevel) {
				nestlevel--;
				NoUnstack--;
				return;
			}
			nestlevel--;
			break;
		}
	}
}

PRIVATE
ifexpr()
{
	/*	ifexpr() returns whether the restricted constant
		expression following #if or #elif evaluates to true.  This
		is done by calling the LLgen generated subparser for
		constant expressions.  The result of this expression will
		be given in the extern long variable "ifval".
	*/
	IMPORT arith ifval;
	int errors = err_occurred;

	ifval = (arith)0;
	AccDefined = 1;
	UnknownIdIsZero = 1;
	PushLex();	/* NEW parser */
	If_expr();	/* invoke constant expression parser	*/
	PopLex();	/* OLD parser */
	AccDefined = 0;
	UnknownIdIsZero = 0;
	return (errors == err_occurred) && (ifval != (arith)0);
}

PRIVATE
do_include()
{
	/*	do_include() performs the inclusion of a file.
	*/
	char *filenm;
	char *result;
	int tok;
	struct token tk;

	AccFileSpecifier = 1;
	if (((tok = GetToken(&tk)) == FILESPECIFIER) || tok == STRING)
		filenm = tk.tk_str;
	else {
		error("bad include syntax");
		filenm = (char *)0;
	}
	AccFileSpecifier = 0;
	PushBack();
	skipline();
	inctable[0] = WorkingDir;
	if (filenm) {
		if (!InsertFile(filenm, &inctable[tok==FILESPECIFIER],&result)){
			if (do_preprocess) error("cannot find include file \"%s\"", filenm);
			else warning("cannot find include file \"%s\"", filenm);
			add_dependency(filenm);
		}
		else {
			add_dependency(result);
			WorkingDir = getwdir(result);
			svnestlevel[++nestcount] = nestlevel;
			FileName = result;
			LineNumber = 1;
		}
	}
}

PRIVATE
do_define()
{
	/*	do_define() interprets a #define control line.
	*/
	char *str;
	int nformals = -1;	/* keep track of the number of formals	*/
	char *formals[NPARAMS];	/* pointers to the names of the formals	*/
	char parbuf[PARBUFSIZE];		/* names of formals	*/
	char *repl_text;	/* start of the replacement text	*/
	int length;		/* length of the replacement text	*/
	register ch;
	char *get_text();

	/* read the #defined macro's name	*/
	if (!(str = GetIdentifier())) {
		error("#define: illegal macro name");
		PushBack();
		skipline();
		return;
	}
	/*	there is a formal parameter list if the identifier is
		followed immediately by a '('. 
	*/
	LoadChar(ch);
	if (ch == '(') {
		if ((nformals = getparams(formals, parbuf)) == -1) {
			PushBack();
			skipline();
			free(str);
			return;	/* an error occurred	*/
		}
		LoadChar(ch);
	}
	/* read the replacement text if there is any			*/
	ch = skipspaces(ch,0);	/* find first character of the text	*/
	assert(ch != EOI);
	if (class(ch) == STNL) {
		/*	Treat `#define something' as `#define something ""'
		*/
		repl_text = "";
		length = 0;
	}
	else {
		PushBack();
		repl_text = get_text((nformals > 0) ? formals : 0, &length);
	}
	macro_def(str2idf(str, 0), repl_text, nformals, length, NOFLAG);
	LineNumber++;
}

PRIVATE
push_if()
{
	if (nestlevel >= IFDEPTH)
		fatal("too many nested #if/#ifdef/#ifndef");
	else
		ifstack[++nestlevel] = 0;
}

PRIVATE
do_elif()
{
	if (nestlevel <= svnestlevel[nestcount] || (ifstack[nestlevel])) {
		error("#elif without corresponding #if");
		skipline();
	}
	else { /* restart at this level as if a #if is detected.  */
		nestlevel--;
		push_if();
		skip_block(1);
	}
}

PRIVATE
do_else()
{
	skipline();
	if (nestlevel <= svnestlevel[nestcount] || (ifstack[nestlevel]))
		error("#else without corresponding #if");
	else {	/* mark this level as else-d		*/
		++(ifstack[nestlevel]);
		skip_block(1);
	}
}

PRIVATE
do_endif()
{
	skipline();
	if (nestlevel <= svnestlevel[nestcount])
		error("#endif without corresponding #if");
	else nestlevel--;
}

PRIVATE
do_if()
{
	push_if();
	if (!ifexpr())	/* a false #if/#elif expression */
		skip_block(0);
}

PRIVATE
do_ifdef(how)
{
	register struct idf *id;
	char *str;

	/*	how == 1 : ifdef; how == 0 : ifndef
	*/
	push_if();
	str = GetIdentifier();
	if (!str) {
		error("illegal #ifdef construction");
		id = 0;
	}
	else	{
		id = findidf(str);
		free(str);
	}

	/* The next test is a shorthand for:
		(how && !id->id_macro) || (!how && id->id_macro)
	*/
	if (how ^ (id && id->id_macro != 0))
		skip_block(0);
	else {
		PushBack();
		skipline();
	}
}

PRIVATE
do_undef()
{
	register struct idf *id;
	register char *str = GetIdentifier();

	/* Forget a macro definition.	*/
	if (str) {
		if (id = findidf(str)) {
			if (id->id_macro) { /* forget the macro */
				free_macro(id->id_macro);
				id->id_macro = (struct macro *) 0;
			} /* else: don't complain */
		}
		free(str);
	}
	else
		error("illegal #undef construction");
	PushBack();
	skipline();
}

PRIVATE
do_line(l)
	unsigned int l;
{
	struct token tk;
	int t = GetToken(&tk);

	PushBack();
	skipline();
	LineNumber = l;		/* the number of the next input line */
	if (t == STRING)	/* is there a filespecifier? */
		FileName = tk.tk_str;
}

PRIVATE int
getparams(buf, parbuf)
	char *buf[];
	char parbuf[];
{
	/*	getparams() reads the formal parameter list of a macro
		definition.
		The number of parameters is returned.
		As a formal parameter list is expected when calling this
		routine, -1 is returned if an error is detected, for
		example:
			#define one(1), where 1 is not an identifier.
		Note that the '(' has already been eaten.
		The names of the formal parameters are stored into parbuf.
	*/
	register char **pbuf = &buf[0];
	register int c;
	register char *ptr = &parbuf[0];
	register char **pbuf2;

	LoadChar(c);
	c = skipspaces(c,0);
	if (c == ')') {		/* no parameters: #define name()	*/
		*pbuf = (char *) 0;
		return 0;
	}
	for (;;) {		/* eat the formal parameter list	*/
		if (class(c) != STIDF) {	/* not an identifier	*/
			error("#define: bad formal parameter");
			return -1;
		}
		*pbuf = ptr;	/* name of the formal	*/
		*ptr++ = c;
		if (ptr >= &parbuf[PARBUFSIZE])
			fatal("formal parameter buffer overflow");
		do {			/* eat the identifier name	*/
			LoadChar(c);
			*ptr++ = c;
			if (ptr >= &parbuf[PARBUFSIZE])
				fatal("formal parameter buffer overflow");
		} while (in_idf(c));
		*(ptr - 1) = '\0';	/* mark end of the name		*/

		/*	Check if this formal parameter is already used.
			Usually, macros do not have many parameters, so ...
		*/
		for (pbuf2 = pbuf - 1; pbuf2 >= &buf[0]; pbuf2--) {
			if (!strcmp(*pbuf2, *pbuf)) {
				warning("formal parameter \"%s\" already used",
					*pbuf);
			}
		}

		pbuf++;
		c = skipspaces(c,0);
		if (c == ')') {	/* end of the formal parameter list	*/
			*pbuf = (char *) 0;
			return pbuf - buf;
		}
		if (c != ',') {
			error("#define: bad formal parameter list");
			return -1;
		}
		LoadChar(c);
		c = skipspaces(c,0);
	}
	/*NOTREACHED*/
}

EXPORT
macro_def(id, text, nformals, length, flags)
	register struct idf *id;
	char *text;
{
	/*	macro_def() puts the contents and information of a macro
		definition into a structure and stores it into the symbol
		table entry belonging to the name of the macro.
		A warning is given if the definition overwrites another.
	*/
	register struct macro *newdef = id->id_macro;
	
	if (newdef) {		/* is there a redefinition?	*/
		if (macroeq(newdef->mc_text, text))
			return;
		warning("redefine \"%s\"", id->id_text);
	}
	else {
#ifdef DOBITS
		register char *p = id->id_text;
#define setbit(bx)	if (!*p) goto go_on; bits[*p++] |= (bx)
		setbit(bit0);
		setbit(bit1);
		setbit(bit2);
		setbit(bit3);
		setbit(bit4);
		setbit(bit5);
		setbit(bit6);
		setbit(bit7);
		
	go_on:
#endif
		id->id_macro = newdef = new_macro();
	}
	newdef->mc_text = text;		/* replacement text	*/
	newdef->mc_nps  = nformals;	/* nr of formals	*/
	newdef->mc_length = length;	/* length of repl. text	*/
	newdef->mc_flag = flags;	/* special flags	*/
	newdef->mc_count = 0;
}

PRIVATE int
find_name(nm, index)
	char *nm, *index[];
{
	/*	find_name() returns the index of "nm" in the namelist
		"index" if it can be found there.  0 is returned if it is
		not there.
	*/
	register char **ip = &index[0];

	while (*ip)
		if (strcmp(nm, *ip++) == 0)
			return ip - &index[0];
	/* arrived here, nm is not in the name list.	*/
	return 0;
}

PRIVATE char *
get_text(formals, length)
	char *formals[];
	int *length;
{
	/*	get_text() copies the replacement text of a macro
		definition with zero, one or more parameters, thereby
		substituting each formal parameter by a special character
		(non-ascii: 0200 & (order-number in the formal parameter
		list)) in order to substitute this character later by the
		actual parameter.  The replacement text is copied into
		itself because the copied text will contain fewer or the
		same amount of characters.  The length of the replacement
		text is returned.

		Implementation:
		finite automaton : we are only interested in
		identifiers, because they might be replaced by some actual
		parameter.  Other tokens will not be seen as such.
	*/
	register int c;
	register unsigned int text_size;
	char *text = Malloc(text_size = ITEXTSIZE);
	register unsigned int pos = 0;
	int match = 0;

	LoadChar(c);

	while ((c != EOI) && (class(c) != STNL)) {
		if (c == '\\') {	/* check for "\\\n"	*/
			LoadChar(c);
			if (c == '\n') {
				/*	More than one line is used for the
					replacement text.
					Replace "\\\n" by " ".
				*/
				text[pos++] = ' ';
				++LineNumber;
			}
			else {
				text[pos++] = '\\';
				if (pos == text_size)
					text = Realloc(text, text_size <<= 1);
				text[pos++] = c;
				if (pos == text_size)
					text = Realloc(text, text_size <<= 1);
			}
			LoadChar(c);
		}
		else
		if (match == 0 && c == '/') {
			LoadChar(c);
			if (c == '*') {
				skipcomment();
				/* text[pos++] = ' '; ??? Why ??? */
				LoadChar(c);
			}
			else
				text[pos++] = '/';
			if (pos == text_size)
				text = Realloc(text, text_size <<= 1);
		}
		else
		if (c == '\'' || c == '"') {
			text[pos++] = c;
			if (pos == text_size)
				text = Realloc(text, text_size <<= 1);
			if (match == c) {
				match = 0;
			} else if (match == 0) {
				match = c;
			}
			LoadChar(c);
		}
		else
		if (match == 0 && formals && class(c) == STIDF) {
			char id_buf[IDFSIZE + 1];
			register char *idp = id_buf;
			int n;

			/* read identifier: it may be a formal parameter */
			*idp++ = c;
			do {
				LoadChar(c);
				if (idp <= &id_buf[IDFSIZE])
					*idp++ = c;
			} while (in_idf(c));
			*--idp = '\0';
			if (n = find_name(id_buf, formals)) {
				/* construct the formal parameter mark	*/
				text[pos++] = FORMALP | (char) n;
				if (pos == text_size)
					text = Realloc(text,
						text_size <<= 1);
			}
			else {
				int sz = idp - id_buf + 1;

				idp = id_buf;

				while (pos + sz >= text_size) text_size <<= 1;
				text = Realloc(text, text_size);
				while (text[pos++] = *idp++) ;
				pos--;
			}
		}
		else {
			text[pos++] = c;
			if (pos == text_size)
				text = Realloc(text, text_size <<= 1);
			LoadChar(c);
		}
	}
	text[pos++] = '\0';
	text = Realloc(text, pos);
	*length = pos - 1;
	return text;
}

#define	BLANK(ch)	((ch == ' ') || (ch == '\t'))

/*	macroeq() decides whether two macro replacement texts are
	identical.  This version compares the texts, which occur
	as strings, without taking care of the leading and trailing
	blanks (spaces and tabs).
*/
PRIVATE
macroeq(s, t)
	register char *s, *t;
{
	
	/* skip leading spaces	*/
	while (BLANK(*s)) s++;
	while (BLANK(*t)) t++;
	/* first non-blank encountered in both strings	*/
	/* The actual comparison loop:			*/
	while (*s && *s == *t)
		s++, t++;
	/* two cases are possible when arrived here:	*/
	if (*s == '\0')	{	/* *s == '\0'		*/
		while (BLANK(*t)) t++;
		return *t == '\0';
	}
	else	{		/* *s != *t		*/
		while (BLANK(*s)) s++;
		while (BLANK(*t)) t++;
		return (*s == '\0') && (*t == '\0');
	}
}
