/* L E X I C A L   A N A L Y S E R   F O R   M O D U L A - 2 */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	"debug.h"
#include	"idfsize.h"
#include	"numsize.h"
#include	"strsize.h"

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>

#include	"input.h"
#include	"f_info.h"
#include	"Lpars.h"
#include	"class.h"
#include	"idf.h"
#include	"type.h"
#include	"LLlex.h"
#include	"const.h"

long str2long();

struct token dot, aside;
struct type *numtype;
struct string string;
int idfsize = IDFSIZE;

static
SkipComment()
{
	/*	Skip Modula-2 comments (* ... *).
		Note that comments may be nested (par. 3.5).
	*/
	register int ch;
	register int NestLevel = 0;

	LoadChar(ch);
	for (;;) {
		if (class(ch) == STNL) {
			LineNumber++;
		}
		else
		if (ch == '(') {
			LoadChar(ch);
			if (ch == '*') {
				++NestLevel;
			}
			else {
				continue;
			}
		}
		else
		if (ch == '*') {
			LoadChar(ch);
			if (ch == ')') {
				if (NestLevel-- == 0) {
					return;
				}
			}
			else {
				continue;
			}
		}
		LoadChar(ch);
	}
}

static
GetString(upto)
{
	/*	Read a Modula-2 string, delimited by the character "upto".
	*/
	register int ch;
	register struct string *str = &string;
	register char *p;
	
	str->s_str = p = Malloc((unsigned int) (str->s_length = ISTRSIZE));
	LoadChar(ch);
	while (ch != upto)	{
		if (class(ch) == STNL)	{
			lexerror("newline in string");
			LineNumber++;
			break;
		}
		if (ch == EOI) {
			lexerror("end-of-file in string");
			break;
		}
		*p++ = ch;
		if (p - str->s_str == str->s_length)	{
			str->s_str = Srealloc(str->s_str,
				(unsigned int) str->s_length + RSTRSIZE);
			p = str->s_str + str->s_length;
			str->s_length += RSTRSIZE;
		}
		LoadChar(ch);
	}
	*p = '\0';
	str->s_length = p - str->s_str;
}

int
LLlex()
{
	/*	LLlex() is the Lexical Analyzer.
		The putting aside of tokens is taken into account.
	*/
	register struct token *tk = &dot;
	char buf[(IDFSIZE > NUMSIZE ? IDFSIZE : NUMSIZE) + 1];
	register int ch, nch;

	numtype = error_type;
	if (ASIDE)	{	/* a token is put aside		*/
		*tk = aside;
		ASIDE = 0;
		return tk->tk_symb;
	}
	tk->tk_lineno = LineNumber;

again:
	LoadChar(ch);
	if ((ch & 0200) && ch != EOI) {
		fatal("non-ascii '\\%03o' read", ch & 0377);
	}

	switch (class(ch))	{

	case STSKIP:
		goto again;

	case STNL:
		LineNumber++;
		tk->tk_lineno++;
		goto again;

	case STGARB:
		if (040 < ch && ch < 0177)	{
			lexerror("garbage char %c", ch);
		}
		else	{
			lexerror("garbage char \\%03o", ch);
		}
		goto again;

	case STSIMP:
		if (ch == '(')	{
			LoadChar(nch);
			if (nch == '*')	{
				SkipComment();
				goto again;
			}
			else	{
				PushBack(nch);
			}
		}
		return tk->tk_symb = ch;

	case STCOMP:
		LoadChar(nch);
		switch (ch)	{

		case '.':
			if (nch == '.')	{
				return tk->tk_symb = UPTO;
			}
			PushBack(nch);
			return tk->tk_symb = ch;

		case ':':
			if (nch == '=')	{
				return tk->tk_symb = BECOMES;
			}
			PushBack(nch);
			return tk->tk_symb = ch;

		case '<':
			if (nch == '=')	{
				return tk->tk_symb = LESSEQUAL;
			}
			if (nch == '>') {
				lexwarning("'<>' is old-fashioned; use '#'");
				return tk->tk_symb = '#';
			}
			PushBack(nch);
			return tk->tk_symb = ch;

		case '>':
			if (nch == '=')	{
				return tk->tk_symb = GREATEREQUAL;
			}
			PushBack(nch);
			return tk->tk_symb = ch;

		default :
			assert(0);
		}

	case STIDF:
	{
		register char *tg = &buf[0];
		register struct idf *id;

		do	{
			if (tg - buf < idfsize) *tg++ = ch;
			LoadChar(ch);
		} while(in_idf(ch));

		if (ch != EOI)
			PushBack(ch);
		*tg++ = '\0';

		tk->TOK_IDF = id = str2idf(buf, 1);
		if (!id) fatal("Out of memory");
		return tk->tk_symb = id->id_reserved ? id->id_reserved : IDENT;
	}

	case STSTR:
		GetString(ch);
		tk->tk_data.tk_str = (struct string *)
				Malloc(sizeof (struct string));
		*(tk->tk_data.tk_str) = string;
		return tk->tk_symb = STRING;

	case STNUM:
	{
		/*	The problem arising with the "parsing" of a number
			is that we don't know the base in advance so we
			have to read the number with the help of a rather
			complex finite automaton.
			Excuses for the very ugly code!
		*/
		register char *np = &buf[1];
					/* allow a '-' to be added	*/

		buf[0] = '-';
		*np++ = ch;
		
		LoadChar(ch);
		while (is_oct(ch))	{
			if (np < &buf[NUMSIZE]) {
				*np++ = ch;
			}
			LoadChar(ch);
		}
		switch (ch) {
		case 'H':
Shex:			*np++ = '\0';
			tk->TOK_INT = str2long(&buf[1], 16);
			if (tk->TOK_INT >= 0 && tk->TOK_INT <= max_int) {
				numtype = intorcard_type;
			}
			else	numtype = card_type;
			return tk->tk_symb = INTEGER;

		case '8':
		case '9':
			do {
				if (np < &buf[NUMSIZE]) {
					*np++ = ch;
				}
				LoadChar(ch);
			} while (is_dig(ch));

			if (is_hex(ch))
				goto S2;
			if (ch == 'H')
				goto Shex;
			if (ch == '.')
				goto Sreal;
			PushBack(ch);
			goto Sdec;

		case 'B':
		case 'C':
			if (np < &buf[NUMSIZE]) {
				*np++ = ch;
			}
			LoadChar(ch);
			if (ch == 'H')
				goto Shex;
			if (is_hex(ch))
				goto S2;
			PushBack(ch);
			ch = *--np;
			*np++ = '\0';
			tk->TOK_INT = str2long(&buf[1], 8);
			if (ch == 'C') {
				numtype = char_type;
				if (tk->TOK_INT < 0 || tk->TOK_INT > 255) {
lexwarning("Character constant out of range");
				}
			}
			else if (tk->TOK_INT >= 0 && tk->TOK_INT <= max_int) {
				numtype = intorcard_type;
			}
			else	numtype = card_type;
			return tk->tk_symb = INTEGER;

		case 'A':
		case 'D':
		case 'E':
		case 'F':
S2:
			do {
				if (np < &buf[NUMSIZE]) {
					*np++ = ch;
				}
				LoadChar(ch);
			} while (is_hex(ch));
			if (ch != 'H') {
				lexerror("H expected after hex number");
				PushBack(ch);
			}
			goto Shex;

		case '.':
Sreal:
			/*	This '.' could be the first of the '..'
				token. At this point, we need a look-ahead
				of two characters.
			*/
			LoadChar(ch);
			if (ch == '.') {
				/*	Indeed the '..' token
				*/
				PushBack(ch);
				PushBack(ch);
				goto Sdec;
			}

			/* a real constant */
			if (np < &buf[NUMSIZE]) {
				*np++ = '.';
			}

			if (is_dig(ch)) {
				/* 	Fractional part
				*/
				do {
					if (np < &buf[NUMSIZE]) {
						*np++ = ch;
					}
					LoadChar(ch);
				} while (is_dig(ch));
			}
			
			if (ch == 'E') {
				/*	Scale factor
				*/
				if (np < &buf[NUMSIZE]) {
					*np++ = 'E';
				}
				LoadChar(ch);
				if (ch == '+' || ch == '-') {
					/*	Signed scalefactor
					*/
					if (np < &buf[NUMSIZE]) {
						*np++ = ch;
					}
					LoadChar(ch);
				}
				if (is_dig(ch)) {
					do {
						if (np < &buf[NUMSIZE]) {
							*np++ = ch;
						}
						LoadChar(ch);
					} while (is_dig(ch));
				}
				else {
					lexerror("bad scale factor");
				}
			}

			PushBack(ch);

			if (np == &buf[NUMSIZE + 1]) {
				lexerror("floating constant too long");
				tk->TOK_REL = Salloc("0.0", 5);
			}
			else {
				tk->TOK_REL = Salloc(buf, np - buf) + 1;
			}
			return tk->tk_symb = REAL;

		default:
			PushBack(ch);
Sdec:
			*np++ = '\0';
			tk->TOK_INT = str2long(&buf[1], 10);
			if (tk->TOK_INT < 0 || tk->TOK_INT > max_int) {
				numtype = card_type;
			}
			else	numtype = intorcard_type;
			return tk->tk_symb = INTEGER;
		}
		/*NOTREACHED*/
	}

	case STEOI:
		return tk->tk_symb = -1;

	case STCHAR:
	default:
		assert(0);
	}
	/*NOTREACHED*/
}
