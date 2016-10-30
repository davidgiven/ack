/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*		    L E X I C A L   A N A L Y Z E R			*/

#include	"parameters.h"

#include	<alloc.h>
#include	"input.h"
#include	"arith.h"
#include	"macro.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"class.h"
#include	"bits.h"

#define	BUFSIZ	1024

struct token dot;

int ReplaceMacros = 1;		/* replacing macros			*/
int AccDefined = 0;		/* accept "defined(...)"		*/
int UnknownIdIsZero = 0;	/* interpret unknown id as integer 0	*/
int Unstacked = 0;		/* an unstack is done			*/
int AccFileSpecifier = 0;	/* return filespecifier <...>		*/
int LexSave = 0;                /* last character read by GetChar       */
extern int InputLevel;		/* # of current macro expansions	*/

extern char	*string_token();
extern arith	char_constant();
#define		FLG_ESEEN	0x01	/* possibly a floating point number */
#define		FLG_DOTSEEN	0x02	/* certainly a floating point number */

int
LLlex()
{
	return (DOT != EOF) ? GetToken(&dot) : EOF;
}


int
GetToken(ptok)
	register struct token *ptok;
{
	/*	GetToken() is the actual token recognizer. It calls the
		control line interpreter if it encounters a "\n{w}*#"
		combination. Macro replacement is also performed if it is
		needed.
	*/
	char buf[BUFSIZ];
	register int ch, nch;

again:	/* rescan the input after an error or replacement	*/
	ch = GetChar();
	/* rescan, the following character has been read	*/
	if ((ch & 0200) && ch != EOI) /* stop on non-ascii character */
		fatal("non-ascii '\\%03o' read", ch & 0377);
	/* keep track of the place of the token in the file	*/

	switch (class(ch)) {	/* detect character class	*/
	case STNL:		/* newline, vertical space or formfeed	*/
		LineNumber++;
		return ptok->tk_symb = EOF;
	case STSKIP:		/* just skip the skip characters	*/
		goto again;
	case STGARB:		/* garbage character			*/
garbage:
		if (040 < ch && ch < 0177)
			error("garbage char %c", ch);
		else
			error("garbage char \\%03o", ch);
		goto again;
	case STSIMP:	/* a simple character, no part of compound token*/
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
			else if (nch == '=')
				return ptok->tk_symb = ANDAB;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '+':
			if (nch == '+')
				return ptok->tk_symb = PLUSPLUS;
			else if (nch == '=')
				return ptok->tk_symb = PLUSAB;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '-':
			if (nch == '-')
				return ptok->tk_symb = MINMIN;
			else if (nch == '>')
				return ptok->tk_symb = ARROW;
			else if (nch == '=')
				return ptok->tk_symb = MINAB;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '<':
			if (AccFileSpecifier) {
				UnGetChar();	/* pushback nch */
				ptok->tk_str =
					string_token("file specifier", '>');
				return ptok->tk_symb = FILESPECIFIER;
			} else if (nch == '<') {
				if ((nch = GetChar()) == '=')
					return ptok->tk_symb = LEFTAB;
				UnGetChar();
				return ptok->tk_symb = LEFT;
			} else if (nch == '=')
				return ptok->tk_symb = LESSEQ;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '=':
			if (nch == '=')
				return ptok->tk_symb = EQUAL;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '>':
			if (nch == '=')
				return ptok->tk_symb = GREATEREQ;
			else if (nch == '>') {
				if ((nch = GetChar()) == '=')
					return ptok->tk_symb = RIGHTAB;
				UnGetChar();
				return ptok->tk_symb = RIGHT;
			}
			UnGetChar();
			return ptok->tk_symb = ch;
		case '|':
			if (nch == '|')
				return ptok->tk_symb = OR;
			else if (nch == '=')
				return ptok->tk_symb = ORAB;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '%':
			if (nch == '=')
				return ptok->tk_symb = MODAB;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '*':
			if (nch == '=')
				return ptok->tk_symb = TIMESAB;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '^':
			if (nch == '=')
				return ptok->tk_symb = XORAB;
			UnGetChar();
			return ptok->tk_symb = ch;
		case '/':
			if (nch == '*' && !InputLevel) {
				skipcomment();
				goto again;
			}
			else if (nch == '=')
				return ptok->tk_symb = DIVAB;
			UnGetChar();
			return ptok->tk_symb = ch;
		default:
			crash("bad class for char 0%o", ch);
			/* NOTREACHED */
		}
	case STCHAR:				/* character constant	*/
		ptok->tk_val = char_constant("character");
		return ptok->tk_symb = INTEGER;
	case STSTR:					/* string	*/
		ptok->tk_str = string_token("string", '"');
		return ptok->tk_symb = STRING;
	case STELL:		/* wide character constant/string prefix */
		nch = GetChar();
		if (nch == '"') {
			ptok->tk_str =
				string_token("wide character string", '"');
			return ptok->tk_symb = STRING;
		} else if (nch == '\'') {
			ptok->tk_val = char_constant("wide character");
			return ptok->tk_symb = INTEGER;
		}
		UnGetChar();
		/* fallthrough */
	case STIDF:
	{
		extern int idfsize;		/* ??? */
		register char *tg = &buf[0];
		register char *maxpos = &buf[idfsize];
		int NoExpandNext = 0;

#define tstmac(bx)	if (!(bits[ch] & bx)) goto nomac
#define cpy		*tg++ = ch
#define load		(ch = GetChar()); if (!in_idf(ch)) goto endidf

		if (Unstacked) EnableMacros();  /* unstack macro's when allowed. */
		if (ch == NOEXPM)  {
			NoExpandNext = 1;
			ch = GetChar();
		}
#ifdef DOBITS
		cpy; tstmac(bit0); load;
		cpy; tstmac(bit1); load;
		cpy; tstmac(bit2); load;
		cpy; tstmac(bit3); load;
		cpy; tstmac(bit4); load;
		cpy; tstmac(bit5); load;
		cpy; tstmac(bit6); load;
		cpy; tstmac(bit7); load;
#endif
		for(;;) {
			if (tg < maxpos) {
				cpy;

			}
			load;
		}
	endidf:
		/*if (ch != EOI) UnGetChar();*/
		UnGetChar();
		*tg++ = '\0';	/* mark the end of the identifier	*/
		if (ReplaceMacros) {
			register struct idf *idef = findidf(buf);

			if (idef && idef->id_macro && !NoExpandNext) {
				if (replace(idef))
					goto again;
			}
		}

	nomac:			/* buf can already be null-terminated. soit */
		ch = GetChar();
		while (in_idf(ch)) {
			if (tg < maxpos) *tg++ = ch;
			ch = GetChar();
		}
		UnGetChar();
		*tg++ = '\0';   /* mark the end of the identifier       */

		NoExpandNext = 0;
		if (UnknownIdIsZero) {
			ptok->tk_val = (arith)0;
			return ptok->tk_symb = INTEGER;
		}
		ptok->tk_str = Malloc((unsigned)(tg - buf));
		strcpy(ptok->tk_str, buf);
		return IDENTIFIER;
	}
	case STNUM:				/* a numeric constant	*/
	{			/* it may only be an integer constant */
		register int base = 10, vch;
		register arith val = 0;
		int ovfl = 0;
		arith ubound = ~(1<<(sizeof(arith)*8-1))/(base/2);

		/* Since the preprocessor only knows integers and has
		 * nothing to do with ellipsis we just return when the
		 * pp-number starts with a '.'
		 */
		if (ch == '.') {
			return ptok->tk_symb = ch;
		}
		if (ch == '0') {
			ch = GetChar();
			if (ch == 'x' || ch == 'X') {
				base = 16;
				ch = GetChar();
			} else {
				base = 8;
			}

		}
		while ((vch = val_in_base(ch, base)) >= 0) {
			if (val < 0 || val > ubound) ovfl++;
			val *= base;
			if (val < 0 && val + vch >= 0) ovfl++;
			val += vch;
			ch = GetChar();
		}
		ptok->tk_unsigned = 0;
		if (ch == 'u' || ch == 'U') {
			ptok->tk_unsigned = 1;
			ch = GetChar();
			if (ch == 'l' || ch == 'L') {
				ch = GetChar();
			}
		}
		else if (ch == 'l' || ch == 'L') {
			ch = GetChar();
			if (ch == 'u' || ch == 'U') {
				ptok->tk_unsigned = 1;
				ch = GetChar();
			}
		}
		if (ovfl) {
			warning("overflow in constant");
			ptok->tk_unsigned = 1;
		}
		else if (val < 0) {
			/* give warning??? */
			ptok->tk_unsigned = 1;
		}
		UnGetChar();
		ptok->tk_val = val;
		return ptok->tk_symb = INTEGER;
	}
	case STEOI:			/* end of text on source file	*/
		return ptok->tk_symb = EOF;
        case STMSPEC:
		if (!InputLevel) goto garbage;
		if (ch == TOKSEP) goto again;
		/* fallthrough shouldn't happen */
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
	do {
		while (c != '*') {
			if (class(c) == STNL) {
				++LineNumber;
			} else if (c == EOI) {
				NoUnstack--;
				return;
			}
			c = GetChar();
		} /* last Character seen was '*' */
		c = GetChar();
	} while (c != '/');
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
		error("%s constant too short", nm);
	else
	while (ch != '\'') {
		if (ch == '\n') {
			error("newline in %s constant", nm);
			LineNumber++;
			break;
		}
		if (ch == '\\')
			ch = quoted(GetChar());
		if (ch >= 128) ch -= 256;
		if (size < sizeof(arith))
			val |= ch << (8 * size);
		size++;
		ch = GetChar();
	}
	if (size > sizeof(arith))
		error("%s constant too long", nm);
	else if (size > 1)
		strict("%s constant includes more than one character", nm);
	return val;
}

char *
string_token(nm, stop_char)
	char *nm;
{
	register int ch;
	register int str_size;
	register char *str = Malloc((unsigned) (str_size = ISTRSIZE));
	register int pos = 0;
	
	ch = GetChar();
	while (ch != stop_char) {
		if (ch == '\n') {
			error("newline in %s", nm);
			LineNumber++;
			break;
		}
		if (ch == EOI) {
			error("end-of-file inside %s", nm);
			break;
		}
		if (ch == '\\' && !AccFileSpecifier)
			ch = quoted(GetChar());
		str[pos++] = ch;
		if (pos == str_size)
			str = Realloc(str, (unsigned)(str_size <<= 1));
		ch = GetChar();
	}
	str[pos++] = '\0'; /* for filenames etc. */
	str = Realloc(str, (unsigned)pos);
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
	} else {				/* a quoted octal */
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
		return is_dig(ch) ? ch - '0' : -1;
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
