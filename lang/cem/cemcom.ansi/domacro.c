/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* PREPROCESSOR: CONTROLLINE INTERPRETER */

#include	"debug.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"idf.h"
#include	"input.h"
#include	"nopp.h"

#ifndef NOPP
#include	"ifdepth.h"
#include	"botch_free.h"
#include	"nparams.h"
#include	"parbufsize.h"
#include	"textsize.h"
#include	"idfsize.h"	
#include	"assert.h"
#include	<alloc.h>
#include	"class.h"
#include	"macro.h"
#include	"macbuf.h"
#include	"replace.h"
#include	"dbsymtab.h"
#ifdef DBSYMTAB
#include	<stb.h>
int		IncludeLevel = 0;
#endif

extern char options[];
extern	char **inctable;	/* list of include directories		*/
extern	char *getwdir();
char ifstack[IFDEPTH];	/* if-stack: the content of an entry is	*/
				/* 1 if a corresponding ELSE has been	*/
				/* encountered.				*/

int	nestlevel = -1;

struct idf *
GetIdentifier(skiponerr)
	int skiponerr;		/* skip the rest of the line on error */
{
	/*	returns a pointer to the descriptor of the identifier that is
		read from the input stream. When the input doe not contain
		an identifier, the rest of the line is skipped when
		skiponerr is on, and a null-pointer is returned.
		The substitution of macros is disabled.
	*/
	int tmp = UnknownIdIsZero;
	int tok;
	struct token tk;

	UnknownIdIsZero = ReplaceMacros = 0;
	tok = GetToken(&tk);
	ReplaceMacros = 1;
	UnknownIdIsZero = tmp;
	if (tok != IDENTIFIER) {
		if (skiponerr && tok != EOI) SkipToNewLine();
		return (struct idf *)0;
	}
	return tk.tk_idf;
}

/*	domacro() is the control line interpreter. The '#' has already
	been read by the lexical analyzer by which domacro() is called.
	The token appearing directly after the '#' is obtained by calling
	the basic lexical analyzing function GetToken() and is interpreted
	to perform the action belonging to that token.
	An error message is produced when the token is not recognized,
	i.e. it is not one of "define" .. "undef" , integer or newline.
*/
domacro()
{
	struct token tk;	/* the token itself			*/
	int toknum;

	EoiForNewline = 1;
	ReplaceMacros = 0;
	toknum = GetToken(&tk);
	ReplaceMacros = 1;
	switch(toknum) {		/* select control line action	*/
	case IDENTIFIER:		/* is it a macro keyword?	*/
		switch (tk.tk_idf->id_resmac) {
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
				lexerror("bad #line syntax");
				SkipToNewLine();
			}
			else
				do_line((unsigned int)tk.tk_ival);
			break;
		case K_ERROR:				/* "error"	*/
			do_error();
			break;
		case K_PRAGMA:				/* "pragma"	*/
			do_pragma();
			break;
		case K_UNDEF:				/* "undef"	*/
			do_undef((struct idf *) 0);
			break;
		default:
			/* invalid word seen after the '#'	*/
			lexerror("%s: unknown control", tk.tk_idf->id_text);
			SkipToNewLine();
		}
		break;
	case INTEGER:		/* # <integer> [<filespecifier>]?	*/
		do_line((unsigned int)tk.tk_ival);
		break;
	case EOI:	/* only `#' on this line: do nothing, ignore	*/
		break;
	default:	/* invalid token following '#'		*/
		lexerror("illegal # line");
		SkipToNewLine();
	}
	EoiForNewline = 0;
}

skip_block(to_endif)
int to_endif;
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
	int toknum;

	NoUnstack++;
	for (;;) {
		ch = GetChar();	/* read first character after newline	*/
		while (class(ch) == STSKIP)
			ch = GetChar();
		if (ch != '#') {
			if (ch == EOI) {
				NoUnstack--;
				return;
			}
			/* A possible '/' is not pushed back */
			if (ch == '/') {
				ch = GetChar();
				if (ch != '*') UnGetChar();
				else {
					skipcomment();
					continue;
				}
			} else UnGetChar();
			SkipToNewLine();
			continue;
		}
		ReplaceMacros = 0;
		toknum = GetToken(&tk);
		ReplaceMacros = 1;
		if (toknum != IDENTIFIER) {
			if (toknum != INTEGER) {
				lexerror("illegal # line");
			}
			SkipToNewLine();
			continue;
		}
		/*	an IDENTIFIER: look for #if, #ifdef and #ifndef
			without interpreting them.
			Interpret #else, #elif and #endif if they occur
			on the same level.
		*/
		switch(tk.tk_idf->id_resmac) {
		default:
		case K_UNKNOWN:
			/* invalid word seen after the '#'      */
			lexwarning("%s: unknown control", tk.tk_idf->id_text);
			/* fallthrough */
		case K_DEFINE:
		case K_ERROR:
		case K_INCLUDE:
		case K_LINE:
		case K_PRAGMA:
		case K_UNDEF:
		case K_FILE:
			SkipToNewLine();
			break;
		case K_IF:
		case K_IFDEF:
		case K_IFNDEF:
			push_if();
			SkipToNewLine();
			break;
		case K_ELIF:
			if (ifstack[nestlevel])
				lexerror("#elif after #else");
			if (!to_endif && nestlevel == skiplevel) {
				nestlevel--;
				push_if();
				if (ifexpr()) {
					NoUnstack--;
					return;
				}
			}
			else SkipToNewLine();	/* otherwise done in ifexpr() */
			break;
		case K_ELSE:
			if (ifstack[nestlevel])
				lexerror("#else after #else");
			++(ifstack[nestlevel]);
			if (!to_endif && nestlevel == skiplevel) {
				if (SkipToNewLine()) {
					if (!options['o'])
						lexstrict("garbage following #else");
				}
				NoUnstack--;
				return;
			}
			else SkipToNewLine();
			break;
		case K_ENDIF:
			ASSERT(nestlevel > nestlow);
			if (nestlevel == skiplevel) {
				if (SkipToNewLine()) {
					if (!options['o'])
						lexstrict("garbage following #endif");
				}
				nestlevel--;
				NoUnstack--;
				return;
			}
			else SkipToNewLine();
			nestlevel--;
			break;
		}
	}
}


ifexpr()
{
	/*	ifexpr() returns whether the restricted constant
		expression following #if or #elif evaluates to true.  This
		is done by calling the LLgen generated subparser for
		constant expressions.  The result of this expression will
		be given in the extern long variable "ifval".
	*/
	extern arith ifval;
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
		filenm = tk.tk_bts;
	else {
		lexerror("bad include syntax");
		filenm = (char *)0;
	}
	AccFileSpecifier = 0;
	if (SkipToNewLine()) {
		lexerror("bad include syntax");
	}
	inctable[0] = WorkingDir;
	if (filenm) {
		if (!InsertFile(filenm, &inctable[tok==FILESPECIFIER],&result)){
			lexerror("cannot open include file \"%s\"", filenm);
			add_dependency(filenm);
		}
		else {
			add_dependency(result);
			WorkingDir = getwdir(result);
			File_Inserted = 1;
			FileName = result;
			LineNumber = 0;
			nestlow = nestlevel;
#ifdef DBSYMTAB
			IncludeLevel++;
			if (options['g']) {
				C_ms_std(FileName, N_BINCL, 0);
			}
#endif /* DBSYMTAB */
		}
	}
}

do_define()
{
	/*	do_define() interprets a #define control line.
	*/
	struct idf *id;		/* the #defined identifier's descriptor	*/
	int nformals = -1;	/* keep track of the number of formals	*/
	char *formals[NPARAMS];	/* pointers to the names of the formals	*/
	char parbuf[PARBUFSIZE];		/* names of formals	*/
	char *repl_text;	/* start of the replacement text	*/
	int length;		/* length of the replacement text	*/
	register ch;
	char *get_text();

	/* read the #defined macro's name	*/
	if (!(id = GetIdentifier(1))) {
		lexerror("illegal #define line");
		return;
	}
	/*	there is a formal parameter list if the identifier is
		followed immediately by a '('.
	*/
	ch = GetChar();
	if (ch == '(') {
		if ((nformals = getparams(formals, parbuf)) == -1) {
			SkipToNewLine();
			return;	/* an error occurred	*/
		}
		ch = GetChar();
	}
	/* read the replacement text if there is any			*/
	ch = skipspaces(ch,0);	/* find first character of the text	*/
	ASSERT(ch != EOI);
	/* UngetChar() is not right when replacement starts with a '/' */
	ChPushBack(ch);
	repl_text = get_text((nformals > 0) ? formals : 0, &length);
	macro_def(id, repl_text, nformals, length, NOFLAG);
	LineNumber++;
}

push_if()
{
	if (nestlevel >= IFDEPTH)
		fatal("too many nested #if/#ifdef/#ifndef");
	else
		ifstack[++nestlevel] = 0;
}

do_elif()
{
	if (nestlevel <= nestlow) {
		lexerror("#elif without corresponding #if");
		SkipToNewLine();
	}
	else {		/* restart at this level as if a #if is detected.  */
		if (ifstack[nestlevel]) {
			lexerror("#elif after #else");
			SkipToNewLine();
		}
		nestlevel--;
		push_if();
		skip_block(1);
	}
}

do_else()
{
	if (SkipToNewLine())
		if (!options['o'])
			lexstrict("garbage following #else");
	if (nestlevel <= nestlow)
		lexerror("#else without corresponding #if");
	else {	/* mark this level as else-d */
		if (ifstack[nestlevel]) {
			lexerror("#else after #else");
		}
		++(ifstack[nestlevel]);
		skip_block(1);
	}
}

do_endif()
{
	if (SkipToNewLine()) {
		if (!options['o'])
			lexstrict("garbage following #endif");
	}
	if (nestlevel <= nestlow)	{
		lexerror("#endif without corresponding #if");
	}
	else	nestlevel--;
}

do_if()
{
	push_if();
	if (!ifexpr())	/* a false #if/#elif expression */
		skip_block(0);
}

do_ifdef(how)
{
	register struct idf *id;

	/*	how == 1 : ifdef; how == 0 : ifndef
	*/
	push_if();
	if (!(id = GetIdentifier(1)))
		lexerror("illegal #ifdef construction");

	/* The next test is a shorthand for:
		(how && !id->id_macro) || (!how && id->id_macro)
	*/
	if (how ^ (id && id->id_macro != 0))
		skip_block(0);
	else if (id)
		SkipToNewLine();
}

/* argidf != NULL when the undef came from a -U option */
do_undef(argidf)
	struct idf *argidf;
{
	register struct idf *id = argidf;

	/* Forget a macro definition.	*/
	if (id || (id = GetIdentifier(1))) {
		if (id->id_macro) { /* forget the macro */
			if (id->id_macro->mc_flag & NOUNDEF) {
				lexerror("it is not allowed to undef %s", id->id_text);
			} else {
				free(id->id_macro->mc_text);
				free_macro(id->id_macro);
				id->id_macro = (struct macro *) 0;
			}
		} /* else: don't complain */
		if (!argidf) {
			if (SkipToNewLine())
				if (!options['o'])
					lexstrict("garbage following #undef");
		}
	}
	else
		lexerror("illegal #undef construction");
}

do_error()
{
	int len;
	char *get_text();
	char *bp = get_text((char **) 0, &len);

	lexerror("user error: %s", bp);
	free(bp);
	LineNumber++;
}

int
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

	c = GetChar();
	c = skipspaces(c,0);
	if (c == ')') {		/* no parameters: #define name()	*/
		*pbuf = (char *) 0;
		return 0;
	}
	for (;;) {		/* eat the formal parameter list	*/
		if (class(c) != STIDF && class(c) != STELL) {
			lexerror("#define: bad formal parameter");
			return -1;
		}
		*pbuf = ptr;	/* name of the formal	*/
		*ptr++ = c;
		if (ptr >= &parbuf[PARBUFSIZE])
			fatal("formal parameter buffer overflow");
		do {			/* eat the identifier name	*/
			c = GetChar();
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
				lexerror("formal parameter \"%s\" already used",
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
			lexerror("#define: bad formal parameter list");
			return -1;
		}
		c = GetChar();
		c = skipspaces(c,0);
	}
	/*NOTREACHED*/
}

macro_def(id, text, nformals, length, flags)
	register struct idf *id;
	char *text;
{
	register struct macro *newdef = id->id_macro;

	/*	macro_def() puts the contents and information of a macro
		definition into a structure and stores it into the symbol
		table entry belonging to the name of the macro.
		An error is given if there was already a definition
	*/
	if (newdef) {		/* is there a redefinition?	*/
		if (newdef->mc_flag & NOUNDEF) {
			lexerror("it is not allowed to redefine %s", id->id_text);
		} else if (!macroeq(newdef->mc_text, text))
			lexerror("illegal redefine of \"%s\"", id->id_text);
		free(text);
		return;
	}
	id->id_macro = newdef = new_macro();
	newdef->mc_text = text;		/* replacement text	*/
	newdef->mc_nps  = nformals;	/* nr of formals	*/
	newdef->mc_length = length;	/* length of repl. text	*/
	newdef->mc_flag = flags;	/* special flags	*/
}

int
find_name(nm, index)
	char *nm, *index[];
{
	/*	find_name() returns the index of "nm" in the namelist
		"index" if it can be found there. 0 is returned if it is
		not there.
	*/
	register char **ip = &index[0];

	while (*ip)
		if (strcmp(nm, *ip++) == 0)
			return ip - &index[0];
	/* arrived here, nm is not in the name list.	*/
	return 0;
}

#define	BLANK(ch)	((ch == ' ') || (ch == '\t'))

char *
get_text(formals, length)
	char *formals[];
	int *length;
{
	/*	get_text() copies the replacement text of a macro
		definition with zero, one or more parameters, thereby
		substituting each formal parameter by a special character
		(non-ascii: 0200 & (order-number in the formal parameter
		list)) in order to substitute this character later by the
		actual parameter. The replacement text is copied into
		itself because the copied text will contain fewer or the
		same amount of characters. The length of the replacement
		text is returned.

		Implementation:
		finite automaton : we are interested in
		1-  white space, sequences must be mapped onto 1 single
		    blank.
		2-  identifiers, since they might be replaced by some
		    actual parameter.
		3-  strings and character constants, since replacing
		    variables within them is illegal, and white-space is
		    significant.
		4-  comment, same as for 1
		Other tokens will not be seen as such.
	*/
	register int c;
	struct repl repls;
	register struct repl *repl = &repls;
	int blank = 0;

	c = GetChar();

	repl->r_ptr = repl->r_text = Malloc(repl->r_size = ITEXTSIZE);
	*repl->r_ptr = '\0';
	while ((c != EOI) && (class(c) != STNL)) {
		if (BLANK(c)) {
			if (!blank++) add2repl(repl, ' ');
			c = GetChar();
			continue;
		}

		if (c == '\'' || c == '"') {
			register int delim = c;

			do {
			    add2repl(repl, c);
			    if (c == '\\') add2repl(repl, GetChar());
			    c = GetChar();
			} while (c != delim && c != EOI && class(c) != STNL);
			if (c == EOI || class(c) == STNL) {
				lexstrict("unclosed opening %c", delim);
				break;
			}
			add2repl(repl, c);
			c = GetChar();
		} else if (c == '/') {
			c = GetChar();
			if (c == '*') {
				skipcomment();
				if (!blank++) add2repl(repl, ' ');
				c = GetChar();
				continue;
			} else add2repl(repl, '/');
		} else if (formals
			    && (class(c) == STIDF || class(c) == STELL)) {
			char id_buf[IDFSIZE + 1];
			register char *idp = id_buf;
			int n;

			/* read identifier: it may be a formal parameter */
			*idp++ = c;
			do {
				c = GetChar();
				if (idp <= &id_buf[IDFSIZE])
					*idp++ = c;
			} while (in_idf(c));
			*--idp = '\0';
			/* construct the formal parameter mark or identifier */
			if (n = find_name(id_buf, formals))
			    add2repl(repl, FORMALP | (char) n);
			else {
			    idp = id_buf;
			    while (*idp) add2repl(repl, *idp++);
			}
		} else if (class(c) == STNUM) {
			add2repl(repl, c);
			if (c == '.') {
				c = GetChar();
				if (class(c) != STNUM) {
					blank = 0; continue;
				}
				add2repl(repl, c);
			}
			c = GetChar();
			while(in_idf(c) || c == '.') {
				add2repl(repl, c);
				if((c = GetChar()) == 'e' || c == 'E') {
					add2repl(repl, c);
					c = GetChar();
					if (c == '+' || c == '-') {
						add2repl(repl, c);
						c = GetChar();
					}
				}
			}
		} else {
			add2repl(repl, c);
			c = GetChar();
		}
		blank = 0;
	}
	*length = repl->r_ptr - repl->r_text;
	return Realloc(repl->r_text, (unsigned)(repl->r_ptr - repl->r_text +1));
}

/*	macroeq() decides whether two macro replacement texts are
	identical.  This version compares the texts, which occur
	as strings, without taking care of the leading and trailing
	blanks (spaces and tabs).
*/
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
#else NOPP

struct idf *
GetIdentifier(skiponerr)
	int skiponerr;		/* skip the rest of the line on error */
{
	/*	returns a pointer to the descriptor of the identifier that is
		read from the input stream. When the input does not contain
		an identifier, the rest of the line is skipped when
		skiponerr is on, and a null-pointer is returned.
		The substitution of macros is disabled.
	*/
	int tok;
	struct token tk;

	tok = GetToken(&tk);
	if (tok != IDENTIFIER) {
		if (skiponerr && tok != EOI) SkipToNewLine();
		return (struct idf *)0;
	}
	return tk.tk_idf;
}

domacro()
{
	int tok;
	struct token tk;

	EoiForNewline = 1;
	if ((tok = GetToken(&tk)) == IDENTIFIER) {
		if (! strcmp(tk.tk_idf->id_text, "pragma")) {
			do_pragma();
			EoiForNewline = 0;
			return;
		}
	} else if (tok == INTEGER) {
		do_line((unsigned int) tk.tk_ival);
		EoiForNewline = 0;
		return;
	}
	lexerror("illegal # line");
	EoiForNewline = 0;
	SkipToNewLine();
}
#endif NOPP


do_line(l)
	unsigned int l;
{
	struct token tk;
	int t = GetToken(&tk);

	if (t != EOI) SkipToNewLine();
	LineNumber = l;		/* the number of the next input line */
	if (t == STRING) {	/* is there a filespecifier? */
		/*
		 * Do not attempt to free the old string, since it might
		 * be used in a def structure.
		 */
		FileName = tk.tk_bts;
	}
}
