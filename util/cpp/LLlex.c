/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*		    L E X I C A L   A N A L Y Z E R			*/

#include	"idfsize.h"
#include	"numsize.h"
#include	"strsize.h"

#include	<alloc.h>
#include	"input.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"class.h"
#include	"bits.h"

/* Data about the token yielded */
struct token dot;

int ReplaceMacros = 1;		/* replacing macros			*/
int AccFileSpecifier = 0;	/* return filespecifier <...>		*/
int AccDefined = 0;		/* accept "defined(...)"		*/
int UnknownIdIsZero = 0;	/* interpret unknown id as integer 0	*/

char *string_token();
char *strcpy();

PushLex()
{
	DOT = 0;
}

PopLex()
{}

int
LLlex()
{
	return (DOT != EOF) ? GetToken(&dot) : EOF;
}

#define BUFSIZ 1024

int
GetToken(ptok)
	register struct token *ptok;
{
	char buf[BUFSIZ];
	register int c, nch;

again:	/* rescan the input after an error or replacement	*/
	LoadChar(c);
	if ((c & 0200) && c != EOI)
		fatal("non-ascii '\\%03o' read", c & 0377);
	switch (class(c)) {	/* detect character class	*/
	case STNL:
		LineNumber++;
		return ptok->tk_symb = EOF;
	case STSKIP:
		goto again;
	case STGARB:		/* garbage character			*/
		if (c == '\\') {
			/* a '\\' is allowed in #if/#elif expression	*/
			LoadChar(c);
			if (class(c) == STNL) {	/* vt , ff ?	*/
				++LineNumber;
				goto again;
			}
			PushBack();
			c = '\\';
		}
		if (040 < c && c < 0177)
			error("garbage char %c", c);
		else
			error("garbage char \\%03o", c);
		goto again;
	case STSIMP:	/* a simple character, no part of compound token*/
		if (c == '/') { /* probably the start of comment	*/
			LoadChar(c);
			if (c == '*') { /* start of comment	*/
				skipcomment();
				goto again;
			}
			else {
				PushBack();
				c = '/';	/* restore c	*/
			}
		}
		return ptok->tk_symb = c;
	case STCOMP:	/* maybe the start of a compound token		*/
		LoadChar(nch);			/* character lookahead	*/
		switch (c) {
		case '!':
			if (nch == '=')
				return ptok->tk_symb = NOTEQUAL;
			PushBack();
			return ptok->tk_symb = c;
		case '&':
			if (nch == '&')
				return ptok->tk_symb = AND;
			PushBack();
			return ptok->tk_symb = c;
		case '<':
			if (AccFileSpecifier) {
				PushBack();	/* pushback nch */
				ptok->tk_str =
					string_token("file specifier", '>');
				return ptok->tk_symb = FILESPECIFIER;
			}
			if (nch == '<')
				return ptok->tk_symb = LEFT;
			if (nch == '=')
				return ptok->tk_symb = LESSEQ;
			PushBack();
			return ptok->tk_symb = c;
		case '=':
			if (nch != '=') {
				PushBack();
				error("missing =");
			}
			return ptok->tk_symb = EQUAL;
		case '>':
			if (nch == '=')
				return ptok->tk_symb = GREATEREQ;
			if (nch == '>')
				return ptok->tk_symb = RIGHT;
			PushBack();
			return ptok->tk_symb = c;
		case '|':
			if (nch == '|')
				return ptok->tk_symb = OR;
			PushBack();
			return ptok->tk_symb = c;
		}
	case STIDF:
	{
		extern int idfsize;		/* ??? */
		register char *tg = &buf[0];
		register char *maxpos = &buf[idfsize];
		register struct idf *idef;

#define tstmac(bx)	if (!(bits[c] & bx)) goto nomac
#define cpy		if (Unstacked) EnableMacros(); *tg++ = c
#define load		LoadChar(c); if (!in_idf(c)) goto endidf

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
		PushBack();
		*tg = '\0';	/* mark the end of the identifier */
		if (ReplaceMacros) {
			idef = findidf(buf);
			if ((idef && idef->id_macro && replace(idef))) {
				goto again;
			}
		}
	nomac:
		LoadChar(c);
		while (in_idf(c)) {
			if (tg < maxpos) *tg++ = c;
			LoadChar(c);
		}
		PushBack();
		*tg++ = '\0';	/* mark the end of the identifier	*/
		if (UnknownIdIsZero) {
			ptok->tk_val = 0;
			return ptok->tk_symb = INTEGER;
		}
		ptok->tk_str = Malloc(tg - buf);
		strcpy(ptok->tk_str, buf);
		return ptok->tk_symb = IDENTIFIER;
	}
	case STCHAR:				/* character constant	*/
	{
		register arith val = 0;
		register int size = 0;

		LoadChar(c);
		if (c == '\'')
			error("character constant too short");
		else
		while (c != '\'') {
			if (c == '\n') {
				error("newline in character constant");
				PushBack();
				break;
			}
			if (c == '\\') {
				LoadChar(c);
				if (c == '\n') {
					LineNumber++;
				}
				c = quoted(c);
			}
			if (c >= 128) c -= 256;
			val = val*256 + c;
			size++;
			LoadChar(c);
		}
		if (size > sizeof(arith))
			error("character constant too long");
		ptok->tk_val = val;
		return ptok->tk_symb = INTEGER;
	}
	case STNUM:
	{
		register char *np = &buf[1];
		register int base = 10;
		register int vch;
		register arith val = 0;

		if (c == '0') {
			*np++ = c;
			LoadChar(c);
			if (c == 'x' || c == 'X') {
				base = 16;
				LoadChar(c);
			}
			else
				base = 8;
		}
		while (vch = val_in_base(c, base), vch >= 0) {
			val = val*base + vch;
			if (np < &buf[NUMSIZE])
				*np++ = c;
			LoadChar(c);
		}
		if (c == 'l' || c == 'L')
			LoadChar(c);
		PushBack();
		ptok->tk_val = val;
		return ptok->tk_symb = INTEGER;
	}
	case STSTR:
		ptok->tk_str = string_token("string", '"');
		return ptok->tk_symb = STRING;
	case STEOI:			/* end of text on source file	*/
		return ptok->tk_symb = EOF;
	default:
		crash("Impossible character class");
	}
	/*NOTREACHED*/
}

skipcomment()
{
	register int c;

	NoUnstack++;
	LoadChar(c);
	do {
		while (c != '*') {
			if (class(c) == STNL)
				++LineNumber;
			else
			if (c == EOI) {
				NoUnstack--;
				return;
			}
			LoadChar(c);
		}
		/* Last Character seen was '*' */
		LoadChar(c);
	} while (c != '/');
	NoUnstack--;
}

char *
string_token(nm, stop_char)
	char *nm;
{
	register int c;
	register unsigned int str_size;
	register char *str = Malloc(str_size = ISTRSIZE);
	register int pos = 0;
	
	LoadChar(c);
	while (c != stop_char) {
		if (c == '\n') {
			error("newline in %s", nm);
			PushBack();
			break;
		}
		if (c == EOI) {
			error("end-of-file inside %s", nm);
			break;
		}
		if (c == '\\') {
			LoadChar(c);
			if (c == '\n') {
				LineNumber++;
				LoadChar(c);
				continue;
			}
			c = quoted(c);
		}
		str[pos++] = c;
		if (pos == str_size)
			str = Realloc(str, str_size <<= 1);
		LoadChar(c);
	}
	str[pos++] = '\0'; /* for filenames etc. */
	str = Realloc(str, pos);
	return str;
}

int
quoted(c)
	register int c;
{	
	/*	quoted() replaces an escaped character sequence by the
		character meant.
	*/
	/* first char after backslash already in c */
	if (!is_oct(c)) {		/* a quoted char */
		switch (c) {
		case 'n':
			c = '\n';
			break;
		case 't':
			c = '\t';
			break;
		case 'b':
			c = '\b';
			break;
		case 'r':
			c = '\r';
			break;
		case 'f':
			c = '\f';
			break;
		}
	}
	else {				/* a quoted octal */
		register int oct = 0, cnt = 0;

		do {
			oct = oct*8 + (c-'0');
			LoadChar(c);
		} while (is_oct(c) && ++cnt < 3);
		PushBack();
		c = oct;
	}
	return c&0377;
}

/* provisional */
int
val_in_base(c, base)
	register int c;
{
	return
		is_dig(c) ? c - '0' :
		base != 16 ? -1 :
		is_hex(c) ? (c - 'a' + 10) & 017 :
		-1;
}
