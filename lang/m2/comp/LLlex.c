/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* L E X I C A L   A N A L Y S E R   F O R   M O D U L A - 2 */

/* $Id$ */

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include "parameters.h"
#include	"debug.h"

#include	"alloc.h"
#include	"em_arith.h"
#include	"em_label.h"
#include	"assert.h"

#include	"LLlex.h"
#include	"input.h"
#include	"f_info.h"
#include	"Lpars.h"
#include	"class.h"
#include	"error.h"
#include	"idf.h"
#include	"def.h"
#include	"type.h"
#include	"warning.h"

extern char *getwdir();

t_token		dot,
		aside;
struct type 		*toktype;
int		idfsize = IDFSIZE;
int		ForeignFlag;
#ifdef DEBUG
extern int	cntlines;
#endif

int	token_nmb = 0;
int	tk_nmb_at_last_syn_err = -ERR_SHADOW;

extern char	options[];
extern int	flt_status;

static void SkipComment(void)
{
	/*	Skip Modula-2 comments (* ... *).
		Note that comments may be nested (par. 3.5).
	*/
	register int ch, c;
	register int CommentLevel = 0;

	LoadChar(ch);
	if (ch == '$') {
		LoadChar(ch);
		switch(ch) {
		case 'F':
			/* Foreign; This definition module has an
			   implementation in another language.
			   In this case, don't generate prefixes in front
			   of the names. Also, don't generate call to
			   initialization routine.
			*/
			ForeignFlag = D_FOREIGN;
			break;
		case 'U':
			inidf['_'] = 1;
			break;
		case 'A': /* Extra array bound checks, on or off */
		case 'R': /* Range checks, on or off */
		{
			int on_on_minus = ch == 'R';
			LoadChar(c);
			if (c == '-') {
				options[ch] = on_on_minus;
				break;
			}
			if (c == '+') {
				options[ch] = !on_on_minus;
				break;
			}
			ch = c;
		}
			/* fall through */
		default:
			break;
		}
	}
	for (;;) {
		if (!(ch & 0200) && class(ch) == STNL) {
			LineNumber++;
#ifdef DEBUG
			cntlines++;
#endif
		}
		else if (ch == '(') {
			LoadChar(ch);
			if (ch == '*') CommentLevel++;
			else continue;
		}
		else if (ch == '*') {
			LoadChar(ch);
			if (ch == ')') {
				CommentLevel--;
				if (CommentLevel < 0) break;
			}
			else continue;
		}
		else if (ch == EOI) {
			lexerror("unterminated comment");
			PushBack();
			break;
		}
		LoadChar(ch);
	}
}

static struct string *GetString(int upto)
{
	/*	Read a Modula-2 string, delimited by the character "upto".
	*/
	register int ch;
	register struct string *str = (struct string *)
			Malloc((unsigned) sizeof(struct string));
	register char *p;
	register int len;
	
	len = ISTRSIZE;
	str->s_str = p = Malloc((unsigned int) ISTRSIZE);
	while (LoadChar(ch), ch != upto)	{
		if (!(ch & 0200) && class(ch) == STNL)	{
			lexerror("newline in string");
			LineNumber++;
#ifdef DEBUG
			cntlines++;
#endif
			break;
		}
		if (ch == EOI)	{
			lexerror("end-of-file in string");
			break;
		}
		*p++ = ch;
		if (p - str->s_str == len)	{
			str->s_str = Realloc(str->s_str,
				(unsigned int) len + RSTRSIZE);
			p = str->s_str + len;
			len += RSTRSIZE;
		}
	}
	str->s_length = p - str->s_str;
	len = (str->s_length+(int)word_size) & ~((int)word_size-1);
	while (p - str->s_str < len) {
		*p++ = '\0';
	}
	str->s_str = Realloc(str->s_str, (unsigned) len);
	if (str->s_length == 0) str->s_length = 1;
	/* ??? string length at least 1 ??? */
	return str;
}

static char *s_error = "illegal line directive";

static int getch(void)
{
	register int ch;

	while (LoadChar(ch), (ch & 0200) && ch != EOI) {
		error("non-ascii '\\%03o' read", ch & 0377);
	}
	return ch;
}

void CheckForLineDirective(void)
{
	register int ch = getch();
	register int	i = 0;
	char		buf[IDFSIZE];
	register char	*c = buf;


	for (;;) {
		if (ch != '#') {
			PushBack();
			return;
		}
		do {	/*
			 * Skip to next digit
			 * Do not skip newlines
			 */
			ch = getch();
			if (class(ch) == STNL || class(ch) == STEOI) {
				LineNumber++;
				error(s_error);
				return;
			}
		} while (class(ch) != STNUM);
		while (class(ch) == STNUM)  {
			i = i*10 + (ch - '0');
			ch = getch();
		}
		while (ch != '"' && class(ch) != STNL && class(ch) != STEOI)
			ch = getch();
		if (ch == '"') {
			c = buf;
			do {
				ch = getch();
				if (c < &buf[IDFSIZE]) *c++ = ch;
				if (class(ch) == STNL || class(ch) == STEOI) {
					LineNumber++;
					error(s_error);
					return;
				}
			} while (ch != '"');
			*--c = '\0';
			do {
				ch = getch();
			} while (class(ch) != STNL && class(ch) != STEOI);
			/*
			 * Remember the file name
			 */
			if (class(ch) == STNL && strcmp(FileName,buf)) {
				FileName = Salloc(buf,(unsigned) strlen(buf) + 1);
				WorkingDir = getwdir(FileName);
			}
		}
		if (class(ch) == STEOI) {
			error(s_error);
			return;
		}
		LineNumber = i;
	}
}

static void CheckForLet()
{
	register int ch;

	LoadChar(ch);
	if (ch != EOI) {
		if (class(ch) == STIDF) {
			lexerror("token separator required between identifier and number");
		}
		PushBack();
	}
}

int LLlex(void)
{
	/*	LLlex() is the Lexical Analyzer.
		The putting aside of tokens is taken into account.
	*/
	register t_token *tk = &dot;
	char buf[(IDFSIZE > NUMSIZE ? IDFSIZE : NUMSIZE) + 2];
	register int ch, nch;

	toktype = error_type;

	if (ASIDE)	{	/* a token is put aside		*/
		*tk = aside;
		ASIDE = 0;
		return tk->tk_symb;
	}

	token_nmb++;
again:
	ch = getch();
	tk->tk_lineno = LineNumber;

	switch (class(ch))	{

	case STNL:
		LineNumber++;
#ifdef DEBUG
		cntlines++;
#endif
		CheckForLineDirective();
		goto again;

	case STSKIP:
		goto again;

	case STGARB:
		if ((unsigned) ch - 040 < 0137)	{
			lexerror("garbage char %c", ch);
		}
		else	lexerror("garbage char \\%03o", ch);
		goto again;

	case STSIMP:
		if (ch == '(')	{
			LoadChar(nch);
			if (nch == '*')	{
				SkipComment();
				goto again;
			}
			PushBack();
		}
		if (ch == '&') return tk->tk_symb = AND;
		if (ch == '~') return tk->tk_symb = NOT;
		return tk->tk_symb = ch;

	case STCOMP:
		LoadChar(nch);
		switch (ch)	{

		case '.':
			if (nch == '.')	{
				return tk->tk_symb = UPTO;
			}
			break;

		case ':':
			if (nch == '=')	{
				return tk->tk_symb = BECOMES;
			}
			break;

		case '<':
			if (nch == '=')	{
				return tk->tk_symb = LESSEQUAL;
			}
			if (nch == '>') {
				return tk->tk_symb = '#';
			}
			break;

		case '>':
			if (nch == '=')	{
				return tk->tk_symb = GREATEREQUAL;
			}
			break;

		default :
			crash("(LLlex, STCOMP)");
		}
		PushBack();
		return tk->tk_symb = ch;

	case STIDF:
	{
		register char *tag = &buf[0];
		register struct idf *id;

		do	{
			if (tag - buf < idfsize) *tag++ = ch;
			LoadChar(ch);
			if (ch == '_' && *(tag-1) == '_') {
				lexerror("an identifier may not contain two consecutive underscores");
			}
		} while(in_idf(ch));

		PushBack();
		*tag = '\0';
		if (*(tag - 1) == '_') {
			lexerror("last character of an identifier may not be an underscore");
		}

		tk->TOK_IDF = id = str2idf(buf, 1);
		return tk->tk_symb = id->id_reserved ? id->id_reserved : IDENT;
	}

	case STSTR: {
		register struct string *str = GetString(ch);

		if (str->s_length == 1) {
			tk->TOK_INT = *(str->s_str) & 0377;
			toktype = char_type;
			free(str->s_str);
			free((char *) str);
		}
		else {
			tk->tk_data.tk_str = str;
			if (! fit((arith)(str->s_length), (int) word_size)) {
				lexerror("string too long");
			}
			toktype = standard_type(T_STRING, 1, (arith)(str->s_length));
		}
		return tk->tk_symb = STRING;
		}

	case STNUM:
	{
		/*	The problem arising with the "parsing" of a number
			is that we don't know the base in advance so we
			have to read the number with the help of a rather
			complex finite automaton.
		*/
		enum statetp {Oct,OptHex,Hex,Dec,OctEndOrHex,End,OptReal,Real};
		register enum statetp state;
		register int base = 8;
		register char *np = &buf[0];

		*np++ = ch;
		state = is_oct(ch) ? Oct : Dec;
		LoadChar(ch);
		for (;;) {
			switch(state) {
			case Oct:
				while (is_oct(ch))	{
					if (np < &buf[NUMSIZE]) *np++ = ch;
					LoadChar(ch);
				}
				if (ch == 'B' || ch == 'C') {
					state = OctEndOrHex;
					break;
				}
				/* Fall Through */
			case Dec:
				base = 10;
				while (is_dig(ch))	{
					if (np < &buf[NUMSIZE]) {
						*np++ = ch;
					}
					LoadChar(ch);
				}
				if (ch == 'D') state = OptHex;
				else if (is_hex(ch)) state = Hex;
				else if (ch == '.') state = OptReal;
				else {
					state = End;
					if (ch == 'H') base = 16;
					else PushBack();
				}
				break;

			case OptHex:
				LoadChar(ch);
				if (is_hex(ch)) {
					if (np < &buf[NUMSIZE]) *np++ = 'D';
					state = Hex;
				}
				else {
					state = End;
					ch = 'D';
					PushBack();
				}
				break;

			case Hex:
				while (is_hex(ch))	{
					if (np < &buf[NUMSIZE]) *np++ = ch;
					LoadChar(ch);
				}
				base = 16;
				state = End;
				if (ch != 'H') {
					lexerror("H expected after hex number");
					PushBack();
				}
				break;

			case OctEndOrHex:
				if (np < &buf[NUMSIZE]) *np++ = ch;
				LoadChar(ch);
				if (ch == 'H') {
					base = 16;
					state = End;
					break;
				}
				if (is_hex(ch)) {
					state = Hex;
					break;
				}
				PushBack();
				ch = *--np;
				*np++ = '\0';
				/* Fall through */
				
			case End: {
				int ovfl = 0;

				*np = '\0';
				if (np >= &buf[NUMSIZE]) {
					tk->TOK_INT = 1;
					lexerror("constant too long");
				}
				else {
					/* The upperbound will be the same as
					   when computed with something like
					   max(unsigned long) / base (when base
					   is even). The problem is that
					   unsigned long or unsigned arith is
					   not accepted by all compilers
					*/
					arith ubound = max_int[sizeof(arith)]
							/ (base >> 1);
					np = &buf[0];
					while (*np == '0') np++;
					tk->TOK_INT = 0;
					while (*np) {
						int c;

						if (is_dig(*np)) {
							c = *np++ - '0';
						}
						else {
							assert(is_hex(*np));
							c = *np++ - 'A' + 10;
						}
						if (tk->TOK_INT < 0 ||
						    tk->TOK_INT > ubound) {
							ovfl++;
						}
						tk->TOK_INT = tk->TOK_INT*base;
						if (tk->TOK_INT < 0 &&
						    tk->TOK_INT + c >= 0) {
							ovfl++;
						}
						tk->TOK_INT += c;
					}
				}
				toktype = card_type;
				if (ch == 'C' && base == 8) {
					toktype = char_type;
					if (ovfl != 0 || tk->TOK_INT>255 ||
					    tk->TOK_INT < 0) {
lexwarning(W_ORDINARY, "character constant out of range");
					}
					CheckForLet();
					return tk->tk_symb = INTEGER;
				}
				if (options['l']) {
					if (base != 10) {
						LoadChar(ch);
						if (ch != 'D') {
							PushBack();
						}
					}
				}
				if (ch == 'D' && (options['l'] || base == 10)) {
				    if (options['l']) {
					/* Local extension: LONGCARD exists,
					   so internally also longintorcard_type
					   exists.
					*/
					toktype = longcard_type;
					if (ovfl == 0 && tk->TOK_INT >= 0 &&
					    tk->TOK_INT<=max_int[(int)long_size]) {
					    toktype = longintorcard_type;
				        }
				        else if (! chk_bounds(tk->TOK_INT,
						      full_mask[(int)long_size],
						      T_CARDINAL)) {
					    ovfl = 1;
				        }
				    }
				    else {
					if (ovfl != 0 ||
					    tk->TOK_INT > max_int[(int)long_size] ||
					    tk->TOK_INT < 0) {
						ovfl = 1;
					}
					toktype = longint_type;
				    }
				}
				else if (ovfl == 0 && tk->TOK_INT >= 0 &&
					 tk->TOK_INT<=max_int[(int)int_size]) {
					toktype = intorcard_type;
				}
				else if (! chk_bounds(tk->TOK_INT,
						      full_mask[(int)int_size],
						      T_CARDINAL)) {
					ovfl = 1;
				}
				if (ovfl)
lexwarning(W_ORDINARY, "overflow in constant");
				CheckForLet();
				return tk->tk_symb = INTEGER;
				}

			case OptReal:
				/*	The '.' could be the first of the '..'
					token. At this point, we need a
					look-ahead of two characters.
				*/
				LoadChar(ch);
				if (ch == '.') {
					/*	Indeed the '..' token
					*/
					PushBack();
					PushBack();
					state = End;
					base = 10;
					break;
				}
				state = Real;
				break;
			}
			if (state == Real) break;
		}

		/* a real real constant */
		if (np < &buf[NUMSIZE]) *np++ = '.';

		toktype = real_type;

		while (is_dig(ch)) {
			/* 	Fractional part
			*/
			if (np < &buf[NUMSIZE]) *np++ = ch;
			LoadChar(ch);
		}

		if (ch == 'D') {
			toktype = longreal_type;
			LoadChar(ch);
			if (ch == '+' || ch == '-' || is_dig(ch)) {
				ch = 'E';
				PushBack();
			}
		}
		if (ch == 'E') {
			/*	Scale factor
			*/
			if (np < &buf[NUMSIZE]) *np++ = ch;
			LoadChar(ch);
			if (ch == '+' || ch == '-') {
				/*	Signed scalefactor
				*/
				if (np < &buf[NUMSIZE]) *np++ = ch;
				LoadChar(ch);
			}
			if (is_dig(ch)) {
				do {
					if (np < &buf[NUMSIZE]) *np++ = ch;
					LoadChar(ch);
				} while (is_dig(ch));
			}
			else {
				lexerror("bad scale factor");
			}
		}

		*np++ = '\0';
		PushBack();

		tk->tk_data.tk_real = new_real();
		if (np >= &buf[NUMSIZE]) {
			tk->TOK_RSTR = Salloc("0.0", 4);
			lexerror("real constant too long");
		}
		else	tk->TOK_RSTR = Salloc(buf, (unsigned) (np - buf));
		CheckForLet();
		flt_str2flt(tk->TOK_RSTR, &(tk->TOK_RVAL));
		if (flt_status == FLT_OVFL) {
lexwarning(W_ORDINARY, "overflow in floating point constant");
		}
		return tk->tk_symb = REAL;

		/*NOTREACHED*/
	}

	case STEOI:
		return tk->tk_symb = -1;

	case STCHAR:
	default:
		crash("(LLlex) Impossible character class");
		/*NOTREACHED*/
	}
	/*NOTREACHED*/
}
