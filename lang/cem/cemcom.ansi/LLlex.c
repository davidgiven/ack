/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*		    L E X I C A L   A N A L Y Z E R			*/

#include	"lint.h"
#include	<alloc.h>
#include	"nofloat.h"
#include	"idfsize.h"
#include	"numsize.h"
#include	"debug.h"
#include	"strsize.h"
#include	"nopp.h"
#include	"input.h"
#include	"arith.h"
#include	"def.h"
#include	"macro.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"class.h"
#include	"assert.h"
#include	"sizes.h"

/* Data about the token yielded */
struct token dot, ahead, aside;

#ifndef NOPP
int ReplaceMacros = 1;		/* replacing macros			*/
int AccDefined = 0;		/* accept "defined(...)"		*/
int UnknownIdIsZero = 0;	/* interpret unknown id as integer 0	*/
int Unstacked = 0;		/* an unstack is done 			*/
#endif
int AccFileSpecifier = 0;	/* return filespecifier <...>		*/
int EoiForNewline = 0;		/* return EOI upon encountering newline	*/
int File_Inserted = 0;		/* a file has just been inserted	*/
int LexSave = 0;		/* last character read by GetChar	*/
#define MAX_LL_DEPTH	2

static struct token LexStack[MAX_LL_DEPTH];
static LexSP = 0;

/*	In PushLex() the actions are taken in order to initialise or
	re-initialise the lexical scanner.
	E.g. at the invocation of a sub-parser that uses LLlex(), the
	state of the current parser should be saved.
*/
PushLex()
{
	ASSERT(LexSP < 2);
	ASSERT(ASIDE == 0);	/* ASIDE = 0;	*/
	GetToken(&ahead);
	LexStack[LexSP++] = dot;
}

PopLex()
{
	ASSERT(LexSP > 0);
	dot = LexStack[--LexSP];
}

int
LLlex()
{
	/*	LLlex() plays the role of Lexical Analyzer for the C parser.
		The look-ahead and putting aside of tokens are taken into
		account.
	*/
	if (ASIDE) {	/* a token is put aside		*/
		dot = aside;
		ASIDE = 0;
	}
	else {		/* read ahead and return the old one	*/
#ifdef	LINT
		lint_comment_ahead();
#endif	LINT
		dot = ahead;
		/*	the following test is performed due to the dual
			task of LLlex(): it is also called for parsing the
			restricted constant expression following a #if or
			#elif.  The newline character causes EOF to be
			returned in this case to stop the LLgen parsing task.
		*/
		if (DOT != EOI)
			GetToken(&ahead);
		else
			DOT = EOF;
	}
	return DOT;
}


char	*string_token();
arith	char_constant();


int
GetToken(ptok)
	register struct token *ptok;
{
	/*	LexToken() is the actual token recognizer. It calls the
		control line interpreter if it encounters a "\n{w}*#"
		combination. Macro replacement is also performed if it is
		needed.
	*/
	char buf[(IDFSIZE > NUMSIZE ? IDFSIZE : NUMSIZE) + 1];
	register int ch, nch;

	if (File_Inserted) {
		File_Inserted = 0;
		goto firstline;
	}

again:	/* rescan the input after an error or replacement	*/
	ch = GetChar();
go_on:	/* rescan, the following character has been read	*/
	if ((ch & 0200) && ch != EOI) /* stop on non-ascii character */
		fatal("non-ascii '\\%03o' read", ch & 0377);
	/* keep track of the place of the token in the file	*/
	ptok->tk_file = FileName;
	ptok->tk_line = LineNumber;

	switch (class(ch)) {	/* detect character class	*/
	case STNL:		/* newline, vertical space or formfeed	*/
firstline:
		LineNumber++;			/* also at vs and ff	*/
		ptok->tk_file = FileName;
		ptok->tk_line = LineNumber;
		if (EoiForNewline)	/* called in control line	*/
			/*	a newline in a control line indicates the
				end-of-information of the line.
			*/
			return ptok->tk_symb = EOI;
		while ((ch = GetChar()), (ch == '#' || class(ch) == STSKIP)) {
			/* blanks are allowed before hashes */
			if (ch == '#') {
				/* a control line follows */
				domacro();
				if (File_Inserted) {
					File_Inserted = 0;
					goto firstline;
				}
			}
		}
			/*	We have to loop here, because in
				`domacro' the nl, vt or ff is read. The
				character following it may again be a `#'.
			*/
		goto go_on;
	case STSKIP:		/* just skip the skip characters	*/
		goto again;
	case STGARB:		/* garbage character			*/
		if (040 < ch && ch < 0177)
			lexerror("garbage char %c", ch);
		else
			lexerror("garbage char \\%03o", ch);
		goto again;
	case STSIMP:	/* a simple character, no part of compound token*/
		if (ch == '/') { /* probably the start of comment	*/
			ch = GetChar();
			if (ch == '*') { /* start of comment */
				skipcomment();
				goto again;
			}
			else {
				UnGetChar();
				ch = '/';	/* restore ch	*/
			}
		}
		return ptok->tk_symb = ch;
	case STCOMP:	/* maybe the start of a compound token		*/
		nch = GetChar();		/* character lookahead	*/
		switch (ch) {
		case '!':
			if (nch == '=')
				return ptok->tk_symb = NOTEQUAL;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '&':
			if (nch == '&')
				return ptok->tk_symb = AND;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '+':
			if (nch == '+')
				return ptok->tk_symb = PLUSPLUS;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '-':
			if (nch == '-')
				return ptok->tk_symb = MINMIN;
			if (nch == '>')
				return ptok->tk_symb = ARROW;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '<':
			if (AccFileSpecifier) {
				UnGetChar();	/* pushback nch */
				ptok->tk_bts = string_token("file specifier",
							'>', &(ptok->tk_len));
				return ptok->tk_symb = FILESPECIFIER;
			}
			if (nch == '<')
				return ptok->tk_symb = LEFT;
			if (nch == '=')
				return ptok->tk_symb = LESSEQ;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '=':
			if (nch == '=')
				return ptok->tk_symb = EQUAL;

 			/*	The following piece of code tries to recognise
 				old-fashioned assignment operators `=op'
				Note however, that these are removed from the
				ANSI C standard.
			*/
 			switch (nch) {
 			case '+':
 				ptok->tk_symb = PLUSAB;
				goto warn;
 			case '-':
 				ptok->tk_symb = MINAB;
				goto warn;
 			case '*':
 				ptok->tk_symb = TIMESAB;
				goto warn;
 			case '/':
 				ptok->tk_symb = DIVAB;
				goto warn;
 			case '%':
 				ptok->tk_symb = MODAB;
				goto warn;
 			case '>':
 			case '<':
 				GetChar(ch);
 				if (ch != nch) {
 					UnGetChar();
 					lexerror("illegal combination '=%c'",
 						nch);
 				}
 				ptok->tk_symb = nch == '<' ? LEFTAB : RIGHTAB;
				goto warn;
 			case '&':
 				ptok->tk_symb = ANDAB;
				goto warn;
 			case '^':
 				ptok->tk_symb = XORAB;
				goto warn;
 			case '|':
 				ptok->tk_symb = ORAB;
			warn:
				warning("Old-fashioned assignment operator");
				return ptok->tk_symb;
 			}
			UnGetChar();
			return ptok->tk_symb = ch;
		case '>':
			if (nch == '=')
				return ptok->tk_symb = GREATEREQ;
			if (nch == '>')
				return ptok->tk_symb = RIGHT;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '|':
			if (nch == '|')
				return ptok->tk_symb = OR;
			UnGetChar();
			return ptok->tk_symb = ch;
		}
	case STCHAR:				/* character constant	*/
		ptok->tk_ival = char_constant("character");
		ptok->tk_fund = INT;
		return ptok->tk_symb = INTEGER;
	case STSTR:					/* string	*/
		ptok->tk_bts = string_token("string", '"', &(ptok->tk_len));
		ptok->tk_fund = CHAR;		/* string of characters */
		return ptok->tk_symb = STRING;
	case STELL:		/* wide character constant/string prefix */
		nch = GetChar();
		if (nch == '"') {
			ptok->tk_bts = string_token("wide character string",
					'"', &(ptok->tk_len));
			ptok->tk_fund = WCHAR;	/* string of wide characters */
			return ptok->tk_symb = STRING;
		} else if (nch == '\'') {
			ptok->tk_ival = char_constant("wide character");
			ptok->tk_fund = INT;
			return ptok->tk_symb = INTEGER;
		}
		UnGetChar();
	case STIDF:
	{
		register char *tg = &buf[0];
		register int pos = -1;
		register int hash;
		register struct idf *idef;
		extern int idfsize;		/* ??? */

		hash = STARTHASH();
		do	{			/* read the identifier	*/
			if (++pos < idfsize) {
				*tg++ = ch;
				hash = ENHASH(hash, ch, pos);
			}
			ch = GetChar();
		} while (in_idf(ch));

		hash = STOPHASH(hash);
		if (ch != EOI)
			UnGetChar();
		*tg++ = '\0';	/* mark the end of the identifier	*/
		idef = ptok->tk_idf = idf_hashed(buf, tg - buf, hash);
		idef->id_file = ptok->tk_file;
		idef->id_line = ptok->tk_line;
#ifndef NOPP
		if (idef->id_macro && ReplaceMacros) {
			if (idef->id_macro->mc_count > 0)
				idef->id_macro->mc_count--;
			else if (replace(idef))
				goto again;
		}
		if (UnknownIdIsZero && idef->id_reserved != SIZEOF) {
			ptok->tk_ival = (arith)0;
			ptok->tk_fund = INT;
			return ptok->tk_symb = INTEGER;
		}
#endif NOPP
		ptok->tk_symb = (
			idef->id_reserved ? idef->id_reserved
			: idef->id_def && idef->id_def->df_sc == TYPEDEF ?
				TYPE_IDENTIFIER
			: IDENTIFIER
		);
		return IDENTIFIER;
	}
	case STNUM:				/* a numeric constant	*/
	{
		register char *np = &buf[1];
		register int base = 10;
		register int vch;
		register arith val = 0;

		if (ch == '.') {
#ifndef NOFLOAT
			/*	A very embarrasing ambiguity. We have either a
				floating point number or field operator or
				ELLIPSIS.
			*/
			ch = GetChar();
			if (!is_dig(ch)) {	/* . or ... */
				if (ch == '.') {
					if ((ch = GetChar()) == '.')
						return ptok->tk_symb = ELLIPSIS;
					/* This is funny: we can't push the
					   second dot back. But then again
					   ..<ch> is already an error in C,
					   so why bother ?
					*/
					UnGetChar();
					lexerror("illegal combination '..'");
				}
				UnGetChar();
				return ptok->tk_symb = '.';
			} else
				*np++ = '0';
			UnGetChar();
#else
			if ((ch = GetChar()) == '.') {
				if ((ch = GetChar()) == '.')
					return ptok->tk_symb = ELLIPSIS;
				UnGetChar();
				lexerror("illegal combination '..'");
			}
			UnGetChar();
			return ptok->tk_symb = '.';
#endif
		}
		if (ch == '0') {
			*np++ = ch;
			ch = GetChar();
			if (ch == 'x' || ch == 'X') {
				base = 16;
				ch = GetChar();
			}
			else
				base = 8;
		}
		while (vch = val_in_base(ch, base), vch >= 0) {
			val = val*base + vch;
			if (np < &buf[NUMSIZE])
				*np++ = ch;
			ch = GetChar();
		}
		if (is_suf(ch)) {
			register int suf_long = 0;
			register int suf_unsigned = 0;

			/*	The type of the integal constant is
				based on its suffix.
			*/
			do {
				switch (ch) {
				case 'l':
				case 'L':
					suf_long++;
					break;
				case 'u':
				case 'U':
					suf_unsigned++;
					break;
				}
				ch = GetChar();
			} while (is_suf(ch));
			UnGetChar();

			if (suf_long > 1)
				lexerror("only one long suffix allowed");
			if (suf_unsigned > 1)
				lexerror("only one unsigned suffix allowed");

			ptok->tk_fund = (suf_long && suf_unsigned) ? ULONG :
					(suf_long) ? LONG : UNSIGNED;
			ptok->tk_ival = val;
			return ptok->tk_symb = INTEGER;
		}
#ifndef NOFLOAT
		if (base == 16 || !(ch == '.' || ch == 'e' || ch == 'E'))
#endif NOFLOAT
		{
			UnGetChar();
			ptok->tk_ival = val;
			/*	The semantic analyser must know if the
				integral constant is given in octal/hexa-
				decimal form, in which case its type is
				UNSIGNED, or in decimal form, in which case
				its type is signed, indicated by
				the fund INTEGER.
			*/
			ptok->tk_fund = 
				(base == 10 || (base == 8 && val == (arith)0))
					? INTEGER : UNSIGNED;
			return ptok->tk_symb = INTEGER;
		}
		/* where's the test for the length of the integral ???	*/
#ifndef NOFLOAT
		if (ch == '.'){
			if (np < &buf[NUMSIZE])
				*np++ = ch;
			ch = GetChar();
		}
		while (is_dig(ch)){
			if (np < &buf[NUMSIZE])
				*np++ = ch;
			ch = GetChar();
		}
		if (ch == 'e' || ch == 'E') {
			if (np < &buf[NUMSIZE])
				*np++ = ch;
			ch = GetChar();
			if (ch == '+' || ch == '-') {
				if (np < &buf[NUMSIZE])
					*np++ = ch;
				ch = GetChar();
			}
			if (!is_dig(ch)) {
				lexerror("malformed floating constant");
				if (np < &buf[NUMSIZE])
					*np++ = ch;
			}
			while (is_dig(ch)) {
				if (np < &buf[NUMSIZE])
					*np++ = ch;
				ch = GetChar();
			}
		}

		/*	The type of an integral floating point
			constant may be given by the float (f)
			or long double (l) suffix.
		*/
		if (ch == 'f' || ch == 'F')
			ptok->tk_fund = FLOAT;
		else if (ch == 'l' || ch == 'L')
			ptok->tk_fund = LNGDBL;
		else {
			ptok->tk_fund = DOUBLE;
			UnGetChar();
		}

		*np++ = '\0';
		buf[0] = '-';	/* good heavens...	*/
		if (np == &buf[NUMSIZE+1]) {
			lexerror("floating constant too long");
			ptok->tk_fval = Salloc("0.0",(unsigned) 5) + 1;
		}
		else
			ptok->tk_fval = Salloc(buf,(unsigned) (np - buf)) + 1;
		return ptok->tk_symb = FLOATING;
#endif NOFLOAT
	}
	case STEOI:			/* end of text on source file	*/
		return ptok->tk_symb = EOI;
	default:				/* this cannot happen	*/
		crash("bad class for char 0%o", ch);
	}
	/*NOTREACHED*/
}

skipcomment()
{
	/*	The last character read has been the '*' of '/_*'.  The
		characters, except NL and EOI, between '/_*' and the first
		occurring '*_/' are not interpreted.
		NL only affects the LineNumber.  EOI is not legal.

		Important note: it is not possible to stop skipping comment
		beyond the end-of-file of an included file.
		EOI is returned by LoadChar only on encountering EOF of the
		top-level file...
	*/
	register int c;

	NoUnstack++;
	c = GetChar();
#ifdef	LINT
	lint_start_comment();
	lint_comment_char(c);
#endif	LINT
	do {
		while (c != '*') {
			if (class(c) == STNL) {
				++LineNumber;
			} else
			if (c == EOI) {
				NoUnstack--;
#ifdef	LINT
				lint_end_comment();
#endif	LINT
				return;
			}
			if (c == '/' && (c = GetChar()) == '*')
				strict("extra comment delimiter found");
			c = GetChar();
#ifdef	LINT
			lint_comment_char(c);
#endif	LINT
		} /* last Character seen was '*' */
		c = GetChar();
#ifdef	LINT
		lint_comment_char(c);
#endif	LINT
	} while (c != '/');
#ifdef	LINT
	lint_end_comment();
#endif	LINT
	NoUnstack--;
}

arith
char_constant(nm)
	char *nm;
{
	register arith val = 0;
	register int ch;
	int size = 0;

	ch = GetChar();
	if (ch == '\'')
		lexerror("%s constant too short", nm);
	else
	while (ch != '\'') {
		if (ch == '\n') {
			lexerror("newline in %s constant", nm);
			LineNumber++;
			break;
		}
		if (ch == '\\')
			ch = quoted(GetChar());
		if (ch >= 128) ch -= 256;
		val = val*256 + ch;
		size++;
		ch = GetChar();
	}
	if (size > 1)
		strict("%s constant includes more than one character", nm);
	if (size > (int)int_size)
		lexerror("%s constant too long", nm);
	return val;
}

char *
string_token(nm, stop_char, plen)
	char *nm;
	int *plen;
{
	register int ch;
	register int str_size;
	register char *str = Malloc((unsigned) (str_size = ISTRSIZE));
	register int pos = 0;
	
	ch = GetChar();
	while (ch != stop_char) {
		if (ch == '\n') {
			lexerror("newline in %s", nm);
			LineNumber++;
			break;
		}
		if (ch == EOI) {
			lexerror("end-of-file inside %s", nm);
			break;
		}
		if (ch == '\\')
			ch = quoted(GetChar());
		str[pos++] = ch;
		if (pos == str_size)
			str = Srealloc(str, (unsigned) (str_size += RSTRSIZE));
		ch = GetChar();
	}
	str[pos++] = '\0'; /* for filenames etc. */
	*plen = pos;
	return str;
}

int
quoted(ch)
	register int ch;
{	
	/*	quoted() replaces an escaped character sequence by the
		character meant.
	*/
	/* first char after backslash already in ch */
	if (!is_oct(ch)) {		/* a quoted char */
		switch (ch) {
		case 'n':
			ch = '\n';
			break;
		case 't':
			ch = '\t';
			break;
		case 'b':
			ch = '\b';
			break;
		case 'r':
			ch = '\r';
			break;
		case 'f':
			ch = '\f';
			break;
		case 'a':		/* alert */
			ch = '\007';
			break;
		case 'v':		/* vertical tab */
			ch = '\013';
			break;
		case 'x':		/* quoted hex */
		{
			register int hex = 0;
			register int vch;

			for (;;) {
				ch = GetChar();
				if (vch = val_in_base(ch, 16), vch == -1)
					break;
				hex = hex * 16 + vch;
			}
			UnGetChar();
			ch = hex;
		}
		}
	}
	else {				/* a quoted octal */
		register int oct = 0, cnt = 0;

		do {
			oct = oct*8 + (ch-'0');
			ch = GetChar();
		} while (is_oct(ch) && ++cnt < 3);
		UnGetChar();
		ch = oct;
	}
	return ch&0377;
}


int
val_in_base(ch, base)
	register int ch;
{
	switch (base) {
	case 8:
		return (is_dig(ch) && ch < '9') ? ch - '0' : -1;
	case 10:
	case 16:
		return is_dig(ch) ? ch - '0'
			: is_hex(ch) ? (ch - 'a' + 10) & 017
			: -1;
	default:
		fatal("(val_in_base) illegal base value %d", base);
		/* NOTREACHED */
	}
}


int
GetChar()
{
	/*	The routines GetChar and trigraph parses the trigraph
		sequences and removes occurences of \\\n.
	*/
	register int ch;

again:
	LoadChar(ch);

	/* possible trigraph sequence */
	if (ch == '?')
		ch = trigraph();

	/* \\\n are removed from the input stream */
	if (ch == '\\') {
		LoadChar(ch);
		if (ch == '\n') {
			++LineNumber;
			goto again;
		}
		PushBack();
		ch = '\\';
	}
	return(LexSave = ch);
}


int
trigraph()
{
	register int ch;

	LoadChar(ch);
	if (ch == '?') {
		LoadChar(ch);
		switch (ch) {		/* its a trigraph */
		case '=':
			ch =  '#';
			return(ch);
		case '(':
			ch = '[';
			return(ch);
		case '/':
			ch = '\\';
			return(ch);
		case ')':
			ch = ']';
			return(ch);
		case '\'':
			ch = '^';
			return(ch);
		case '<':
			ch = '{';
			return(ch);
		case '!':
			ch = '|';
			return(ch);
		case '>':
			ch = '}';
			return(ch);
		case '-':
			ch = '~';
			return(ch);
		}
		PushBack();
	}
	PushBack();
	return('?');
}
