/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* L E X I C A L   A N A L Y S E R   F O R   M O D U L A - 2 */

/* $Id$ */

#include	<alloc.h>
#include	"idfsize.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"input.h"
#include	"f_info.h"
#include	"Lpars.h"
#include	"class.h"

struct token	dot,
		aside;
int		idfsize = IDFSIZE;
int		ForeignFlag;

extern char	*getwdir();

static
SkipComment()
{
	/*	Skip Modula-2 comments (* ... *).
		Note that comments may be nested (par. 3.5).
	*/
	register int ch;
	register int CommentLevel = 0;

	LoadChar(ch);
	if (ch == '$') {
		LoadChar(ch);
		switch(ch) {
		case 'F':
			/* Foreign; This definition module has an
			   implementation in another language.
			   In this case, check that the object file is present
			   and don't generate a rule for it.
			*/
			ForeignFlag = 1;
			break;
		default:
			PushBack();
			break;
		}
	}
	for (;;) {
		if (class(ch) == STNL) {
			LineNumber++;
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
			break;
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
	
	while (LoadChar(ch), ch != upto)	{
		if (class(ch) == STNL)	{
			lexerror("newline in string");
			LineNumber++;
			break;
		}
		if (ch == EOI)	{
			lexerror("end-of-file in string");
			break;
		}
	}
}

static char *s_error = "illegal line directive";

static int
getch()
{
	register int ch;

	for (;;) {
		LoadChar(ch);
		if ((ch & 0200) && ch != EOI) {
			error("non-ascii '\\%03o' read", ch & 0377);
			continue;
		}
		break;
	}
	return ch;
}

CheckForLineDirective()
{
	register int ch = getch();
	register int	i = 0;
	char		buf[IDFSIZE + 2];
	register char	*c = buf;


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
			*c++ = ch = getch();
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

char idfbuf[IDFSIZE + 2];

int
LLlex()
{
	/*	LLlex() is the Lexical Analyzer.
		The putting aside of tokens is taken into account.
	*/
	register struct token *tk = &dot;
	register int ch, nch;

	if (ASIDE)	{	/* a token is put aside		*/
		*tk = aside;
		ASIDE = 0;
		return tk->tk_symb;
	}

again:
	ch = getch();
	tk->tk_lineno = LineNumber;

	switch (class(ch))	{

	case STNL:
		LineNumber++;
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
		register char *tag = &idfbuf[0];
		register struct idf *id;

		do	{
			if (tag - idfbuf < idfsize) *tag++ = ch;
			LoadChar(ch);
		} while(in_idf(ch));

		PushBack();
		*tag++ = '\0';

		tk->TOK_IDF = id = findidf(idfbuf);
		return tk->tk_symb = id && id->id_reserved ? id->id_reserved : IDENT;
	}

	case STSTR:
		GetString(ch);
		return tk->tk_symb = STRING;

	case STNUM:
	{
		/*	The problem arising with the "parsing" of a number
			is that we don't know the base in advance so we
			have to read the number with the help of a rather
			complex finite automaton.
		*/
		enum statetp {Oct,OptHex,Hex,Dec,OctEndOrHex,End,OptReal,Real};
		register enum statetp state;
		state = is_oct(ch) ? Oct : Dec;
		LoadChar(ch);
		for (;;) {
			switch(state) {
			case Oct:
				while (is_oct(ch))	{
					LoadChar(ch);
				}
				if (ch == 'B' || ch == 'C') {
					state = OctEndOrHex;
					break;
				}
				/* Fall Through */
			case Dec:
				while (is_dig(ch))	{
					LoadChar(ch);
				}
				if (ch == 'D') state = OptHex;
				else if (is_hex(ch)) state = Hex;
				else if (ch == '.') state = OptReal;
				else {
					state = End;
					if (ch != 'H') PushBack();
				}
				break;

			case OptHex:
				LoadChar(ch);
				if (is_hex(ch)) {
					state = Hex;
				}
				else {
					ch = 'D';
					state = End;
					PushBack();
				}
				break;

			case Hex:
				while (is_hex(ch))	{
					LoadChar(ch);
				}
				state = End;
				if (ch != 'H') {
					lexerror("H expected after hex number");
					PushBack();
				}
				break;

			case OctEndOrHex:
				LoadChar(ch);
				if (ch == 'H') {
					state = End;
					break;
				}
				if (is_hex(ch)) {
					state = Hex;
					break;
				}
				PushBack();
				/* Fall through */
				
			case End:
				return tk->tk_symb = INTEGER;

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
					break;
				}
				state = Real;
				break;
			}
			if (state == Real) break;
		}

		while (is_dig(ch)) {
			/* 	Fractional part
			*/
			LoadChar(ch);
		}

		if (ch == 'E' || ch == 'D') {
			/*	Scale factor
			*/
			if (ch == 'D') {
				LoadChar(ch);
				if (!(ch == '+' || ch == '-' || is_dig(ch)))
					goto noscale;
			}
			LoadChar(ch);
			if (ch == '+' || ch == '-') {
				/*	Signed scalefactor
				*/
				LoadChar(ch);
			}
			if (is_dig(ch)) {
				do {
					LoadChar(ch);
				} while (is_dig(ch));
			}
			else {
				lexerror("bad scale factor");
			}
		}

noscale:
		PushBack();

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
