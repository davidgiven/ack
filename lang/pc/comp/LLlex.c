/* L E X I C A L   A N A L Y S E R   F O R   I S O - P A S C A L */

#include	"debug.h"
#include	"idfsize.h"
#include	"numsize.h"
#include	"strsize.h"

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"Lpars.h"
#include	"class.h"
#include	"const.h"
#include	"f_info.h"
#include	"idf.h"
#include	"input.h"
#include	"main.h"
#include	"type.h"

extern long	str2long();
extern char	*Malloc();

#define	TO_LOWER(ch)	(ch |= ( ch>='A' && ch<='Z' ) ? 0x0020 : 0)

#ifdef DEBUG
extern int cntlines;
#endif

int idfsize = IDFSIZE;
struct token	dot,
		aside;

struct type	*toktype,
		*asidetype;

static int	eofseen;

STATIC
SkipComment()
{
	/*	Skip ISO-Pascal comments (* ... *) or { ... }.
		Note :
			comments may not be nested (ISO 6.1.8).
			(* and { are interchangeable, so are *) and }.
	*/
	register int ch;

	LoadChar(ch);
	for (;;)	{
		if( class(ch) == STNL )	{
			LineNumber++;
#ifdef DEBUG
			cntlines++;
#endif
		}
		else if( ch == '*' )	{
			LoadChar(ch);
			if( ch == ')' ) return;		/* *) */
			else continue;
		}
		else if( ch == '}' ) return;
		else if( ch == EOI )	{
			lexerror("unterminated comment");
			break;
		}
		LoadChar(ch);
	}
}

STATIC struct string *
GetString()
{
	/*	Read a Pascal string, delimited by the character "'".
	*/
	register int ch;
	register struct string *str = (struct string *)
				Malloc((unsigned) sizeof(struct string));
	register char *p;
	register int len = ISTRSIZE;
	
	str->s_str = p = Malloc((unsigned int) ISTRSIZE);
	for( ; ; )	{
		LoadChar(ch);
		if( ch & 0200 )
			fatal("non-ascii '\\%03o' read", ch & 0377);
			/*NOTREACHED*/
		if( class(ch) == STNL )	{
			lexerror("newline in string");
			LineNumber++;
#ifdef DEBUG
			cntlines++;
#endif
			break;
		}
		if( ch == EOI )	{
			lexerror("end-of-file in string");
			break;
		}
		if( ch == '\'' )	{
			LoadChar(ch);
			if( ch != '\'' )
				break;
		}
		*p++ = ch;
		if( p - str->s_str == len )	{
			extern char *Srealloc();

			str->s_str = Srealloc(str->s_str,
					(unsigned int) len + RSTRSIZE);
			p = str->s_str + len;
			len += RSTRSIZE;
		}
	}
	if( ch == EOI ) eofseen = 1;
	else PushBack();

	str->s_length = p - str->s_str;
	*p++ = '\0';

	/* ISO 6.1.7: string length at least 1 */
	if( str->s_length == 0 )	{
		lexerror("character-string: at least one character expected");
		str->s_length = 1;
	}

	return str;
}

int
LLlex()
{
	/*	LLlex() is the Lexical Analyzer.
		The putting aside of tokens is taken into account.
	*/
	register struct token *tk = &dot;
	register int ch, nch;

	toktype = error_type;

	if( ASIDE )	{	/* a token is put aside */
		*tk = aside;
		toktype = asidetype;
		ASIDE = 0;
		return tk->tk_symb;
	}

	tk->tk_lineno = LineNumber;

	if( eofseen )	{
		eofseen = 0;
		ch = EOI;
	}
	else	{
again:
		LoadChar(ch);
		if( !options['C'] )		/* -C : cases are different */
			TO_LOWER(ch);

		if( (ch & 0200) && ch != EOI )
			fatal("non-ascii '\\%03o' read", ch & 0377);
			/*NOTREACHED*/
	}

	switch( class(ch) )	{

	case STNL:
		LineNumber++;
		tk->tk_lineno++;
#ifdef DEBUG
		cntlines++;
#endif
		goto again;

	case STSKIP:
		goto again;

	case STGARB:
		if( (unsigned) ch < 0177 )
			lexerror("garbage char %c", ch);
		else
			crash("(LLlex) garbage char \\%03o", ch);
		goto again;

	case STSIMP:
		if( ch == '(' )	{
			LoadChar(nch);
			if( nch == '*' )	{		/* (* */
				SkipComment();
				tk->tk_lineno = LineNumber;
				goto again;
			}
			if( nch == '.' )			/* (. is [ */
				return tk->tk_symb = '[';
			if( nch == EOI ) eofseen = 1;
			else PushBack();
		}
		else if( ch == '{' )	{
			SkipComment();
			tk->tk_lineno = LineNumber;
			goto again;
		}
		else if( ch == '@' ) ch = '^';		/* @ is ^ */

		return tk->tk_symb = ch;

	case STCOMP:
		LoadChar(nch);
		switch( ch )	{

		case '.':
			if( nch == '.' )			/* .. */
				return tk->tk_symb = UPTO;
			if( nch == ')' )			/* .) is ] */
				return tk->tk_symb = ']';
			break;

		case ':':
			if( nch == '=' )			/* := */
				return tk->tk_symb = BECOMES;
			break;

		case '<':
			if( nch == '=' )			/* <= */
				return tk->tk_symb = LESSEQUAL;
			if( nch == '>' )			/* <> */
				return tk->tk_symb = NOTEQUAL;
			break;

		case '>':
			if( nch == '=' )			/* >= */
				return tk->tk_symb = GREATEREQUAL;
			break;

		default :
			crash("(LLlex, STCOMP)");
			/*NOTREACHED*/
		}
		if( nch == EOI ) eofseen = 1;
		else PushBack();
		return tk->tk_symb = ch;

	case STIDF:	{
		char buf[IDFSIZE + 1];
		register char *tag = &buf[0];
		register struct idf *id;
		extern struct idf *str2idf();

		do	{
			if( !options['C'] )	/* -C : cases are different */
				TO_LOWER(ch);
			if( tag - buf < idfsize )
				*tag++ = ch;
			LoadChar(ch);
		} while( in_idf(ch) );
		*tag = '\0';

		if( ch == EOI ) eofseen = 1;
		else PushBack();

		tk->TOK_IDF = id = str2idf(buf, 1);
		return tk->tk_symb = id->id_reserved ? id->id_reserved : IDENT;
	}

	case STSTR:	{
		register struct string *str = GetString();

		if( str->s_length == 1 )	{
#ifdef DEBUG
			if( options['l'] )	{
				/* to prevent LexScan from crashing */
				tk->tk_data.tk_str = str;
				return tk->tk_symb = STRING;
			}
#endif
			tk->TOK_INT = *(str->s_str) & 0377;
			toktype = char_type;
			free(str->s_str);
			free((char *) str);
		}
		else	{
			tk->tk_data.tk_str = str;
			toktype = standard_type(T_STRING, 1, str->s_length);
		}
		return tk->tk_symb = STRING;
	}

	case STNUM:	{
#define INT_MODE	0
#define REAL_MODE	1

		char buf[NUMSIZE+2];
		register char *np = buf;
		register int state = INT_MODE;
		extern char *Salloc();

		do	{
			if( np <= &buf[NUMSIZE] )
				*np++ = ch;
			LoadChar(ch);
		} while( is_dig(ch) );

		if( ch == '.' )	{
			LoadChar(ch);
			if( is_dig(ch) )	{
				if( np <= &buf[NUMSIZE] )
					*np++ = '.';
				do	{
					/* fractional part */
					if( np <= &buf[NUMSIZE] )
						*np++ = ch;
					LoadChar(ch);
				} while( is_dig(ch) );
				state = REAL_MODE;
			}
			else	{
				PushBack();
				PushBack();
				goto end;
			}
				
		}
		if( ch == 'e' || ch == 'E' )	{
			char *tp = np;		/* save position in string */

			/* scale factor */
			if( np <= &buf[NUMSIZE] )
				*np++ = ch;
			LoadChar(ch);
			if( ch == '+' || ch == '-' )	{
				/* signed scale factor */
				if( np <= &buf[NUMSIZE] )
					*np++ = ch;
				LoadChar(ch);
			}
			if( is_dig(ch) )	{
				do	{
					if( np <= &buf[NUMSIZE] )
						*np++ = ch;
					LoadChar(ch);
				} while( is_dig(ch) );
				state = REAL_MODE;
			}
			else	{
				PushBack();
				PushBack();
				if( np - tp == 2 )	/* sign */
					PushBack();
				np = tp;		/* restore position */
				goto end;
			}
		}
		/* syntax of number is correct */
		if( ch == EOI ) eofseen = 1;
		else PushBack();
	end:
		*np++ = '\0';

		if( state == INT_MODE )	{
			if( np > &buf[NUMSIZE+1] )	{
				tk->TOK_INT = 1;
				lexerror("constant too long");
			}
			else	{
				np = buf;
				while (*np == '0')	/* skip leading zeros */
					np++;
				tk->TOK_INT = str2long(np, 10);
				if( tk->TOK_INT < 0 ||
				    strlen(np) > strlen(maxint_str) ||
					strlen(np) == strlen(maxint_str) &&
					strcmp(np, maxint_str) > 0 )
					     lexwarning("overflow in constant");
			}
			toktype = int_type;
			return tk->tk_symb = INTEGER;
		}
		/* REAL_MODE */
		tk->tk_data.tk_real = (struct real *)
						Malloc(sizeof(struct real));
		/* allocate struct for inverse */
		tk->TOK_RIV = (struct real *) Malloc(sizeof(struct real));
		tk->TOK_RIV->r_inverse = tk->tk_data.tk_real;

		/* sign */
		tk->TOK_RSI = 0;
		tk->TOK_RIV->r_sign = 1;

		if( np > &buf[NUMSIZE+1] )	{
			tk->TOK_REL = Salloc("0.0", 4);
			lexerror("floating constant too long");
		}
		else tk->TOK_REL = Salloc(buf, np - buf);

		toktype = real_type;
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
