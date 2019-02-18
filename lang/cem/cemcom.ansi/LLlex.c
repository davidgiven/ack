/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*		    L E X I C A L   A N A L Y Z E R			*/

#include <assert.h>
#include <alloc.h>
#include "parameters.h"
#include "input.h"
#include "arith.h"
#include "def.h"
#include "macro.h"
#include "idf.h"
#include "LLlex.h"
#include "Lpars.h"
#include "class.h"
#include "sizes.h"
#include "error.h"
#include "domacro.h"
#include "specials.h" /* registration of special identifiers */

/* Data about the token yielded */
struct token dot, ahead, aside;
int token_nmb = 0; /* number of the ahead token */
int tk_nmb_at_last_syn_err = -5 /*ERR_SHADOW*/;
/* token number at last syntax error */
int idfsize = IDFSIZE;
char sp_occurred[SP_TOTAL + 1];

int AccFileSpecifier = 0; /* return filespecifier <...>		*/
int EoiForNewline = 0; /* return EOI upon encountering newline	*/
int File_Inserted = 0; /* a file has just been inserted	*/
int LexSave = 0; /* last character read by GetChar	*/
#define MAX_LL_DEPTH 2

#define FLG_ESEEN 0x01 /* possibly a floating point number */
#define FLG_DOTSEEN 0x02 /* certainly a floating point number */
extern arith full_mask[];

#ifdef LINT
extern int lint_skip_comment;
#endif


/* Internal function declarations */
static arith char_constant(char*);
static char* string_token(char *, int , int *);
static int quoted(register int);
static int hex_val(register int);
static void strflt2tok(char [], struct token *);
static void strint2tok(char [], struct token *);

int LLlex(void)
{
	/*	LLlex() plays the role of Lexical Analyzer for the C parser.
	    The look-ahead and putting aside of tokens are taken into
	    account.
	*/
	if (ASIDE)
	{ /* a token is put aside		*/
		dot = aside;
		ASIDE = 0;
	}
	else
	{ /* read ahead and return the old one	*/
#ifdef LINT
		lint_comment_ahead();
#endif /* LINT */
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


int GetToken(register struct token* ptok)
{
	/*	GetToken() is the actual token recognizer. It calls the
	    control line interpreter if it encounters a "\n{w}*#"
	    combination. Macro replacement is also performed if it is
	    needed.
	*/
	char buf[(IDFSIZE > NUMSIZE ? IDFSIZE : NUMSIZE) + 1];
	register int ch, nch;

	token_nmb++;

	if (File_Inserted)
	{
		File_Inserted = 0;
		goto firstline;
	}

again: /* rescan the input after an error or replacement	*/
	ch = GetChar();
go_on: /* rescan, the following character has been read	*/
	if ((ch & 0200) && ch != EOI) /* stop on non-ascii character */
	{
		fatal("non-ascii '\\%03o' read", ch & 0377);
	}
	/* keep track of the place of the token in the file	*/
	ptok->tk_file = FileName;
	ptok->tk_line = LineNumber;

	switch (class(ch))
	{ /* detect character class	*/
		case STNL: /* newline, vertical space or formfeed	*/
		firstline:
			LineNumber++; /* also at vs and ff	*/
			ptok->tk_file = FileName;
			ptok->tk_line = LineNumber;
			if (EoiForNewline) /* called in control line	*/
				/*	a newline in a control line indicates the
				    end-of-information of the line.
				*/
				return ptok->tk_symb = EOI;

			while ((ch = GetChar()), (ch == '#'
			                          || class(ch) == STSKIP))
			{
				/* blanks are allowed before hashes */
				if (ch == '#')
				{
					/* a control line follows */
					domacro();
				}
			}
			/*	We have to loop here, because in
			    `domacro' the nl, vt or ff is read. The
			    character following it may again be a `#'.
			*/
			goto go_on;
		case STSKIP: /* just skip the skip characters	*/
			goto again;
		case STGARB: /* garbage character			*/
			if (040 < ch && ch < 0177)
			{
				return ptok->tk_symb = ch;
			}
			else
			{
				lexerror("garbage char \\%03o", ch);
			}
			goto again;
		case STSIMP: /* a simple character, no part of compound token*/
			return ptok->tk_symb = ch;
		case STCOMP: /* maybe the start of a compound token		*/
			nch = GetChar(); /* character lookahead	*/
			switch (ch)
			{
				case '!':
					if (nch == '=')
						return ptok->tk_symb = NOTEQUAL;
					break;
				case '&':
					if (nch == '&')
						return ptok->tk_symb = AND;
					if (nch == '=')
						return ptok->tk_symb = ANDAB;
					break;
				case '+':
					if (nch == '+')
						return ptok->tk_symb = PLUSPLUS;
					if (nch == '=')
						return ptok->tk_symb = PLUSAB;
					break;
				case '-':
					if (nch == '-')
						return ptok->tk_symb = MINMIN;
					if (nch == '>')
						return ptok->tk_symb = ARROW;
					if (nch == '=')
						return ptok->tk_symb = MINAB;
					break;
				case '<':
					if (AccFileSpecifier)
					{
						UnGetChar(); /* pushback nch */
						ptok->tk_bts = string_token("file specifier", '>', &(ptok->tk_len));
						return ptok->tk_symb = FILESPECIFIER;
					}
					if (nch == '<')
					{
						if ((nch = GetChar()) == '=')
							return ptok->tk_symb = LEFTAB;
						UnGetChar();
						return ptok->tk_symb = LEFT;
					}
					if (nch == '=')
						return ptok->tk_symb = LESSEQ;
					break;
				case '=':
					if (nch == '=')
						return ptok->tk_symb = EQUAL;
					break;
				case '>':
					if (nch == '=')
						return ptok->tk_symb = GREATEREQ;
					if (nch == '>')
					{
						if ((nch = GetChar()) == '=')
							return ptok->tk_symb = RIGHTAB;
						UnGetChar();
						return ptok->tk_symb = RIGHT;
					}
					break;
				case '|':
					if (nch == '|')
						return ptok->tk_symb = OR;
					if (nch == '=')
						return ptok->tk_symb = ORAB;
					break;
				case '%':
					if (nch == '=')
						return ptok->tk_symb = MODAB;
					break;
				case '*':
					if (nch == '=')
						return ptok->tk_symb = TIMESAB;
					break;
				case '^':
					if (nch == '=')
						return ptok->tk_symb = XORAB;
					break;
				case '/':
					if (nch == '=')
						return ptok->tk_symb = DIVAB;
					break;
				default:
					crash("bad class for char 0%o", ch);
					/* NOTREACHED */
			}
			UnGetChar();
			return ptok->tk_symb = ch;
		case STCHAR: /* character constant	*/
			ptok->tk_ival = char_constant("character");
			ptok->tk_fund = INT;
			return ptok->tk_symb = INTEGER;
		case STSTR: /* string	*/
			ptok->tk_bts = string_token("string", '"', &(ptok->tk_len));
			ptok->tk_fund = CHAR; /* string of characters */
			return ptok->tk_symb = STRING;
		case STELL: /* wide character constant/string prefix */
			nch = GetChar();
			if (nch == '"')
			{
				ptok->tk_bts = string_token("wide character string", '"', &(ptok->tk_len));
				ptok->tk_fund = WCHAR; /* string of wide characters */
				return ptok->tk_symb = STRING;
			}
			else if (nch == '\'')
			{
				ptok->tk_ival = char_constant("wide character");
				ptok->tk_fund = INT;
				return ptok->tk_symb = INTEGER;
			}
			UnGetChar();
		/* fallthrough */
		case STIDF:
		{
			register char* tg = &buf[0];
			register int pos = -1;
			register struct idf* idef;
			extern int idfsize; /* ??? */
			do
			{ /* read the identifier	*/
				if (++pos < idfsize)
				{
					*tg++ = ch;
				}
				ch = GetChar();
			} while (in_idf(ch));

			if (ch != EOI)
				UnGetChar();
			*tg++ = '\0'; /* mark the end of the identifier	*/
			idef = ptok->tk_idf = str2idf(buf, 1);
			sp_occurred[idef->id_special] = 1;
			idef->id_file = ptok->tk_file;
			idef->id_line = ptok->tk_line;
			ptok->tk_symb
			    = (idef->id_reserved
			           ? idef->id_reserved
			           : idef->id_def && idef->id_def->df_sc == TYPEDEF ? TYPE_IDENTIFIER
			                                                            : IDENTIFIER);
			return IDENTIFIER;
		}
		case STNUM: /* a numeric constant	*/
		{
			register int siz_left = NUMSIZE - 1;
			register char* np = &buf[0];
			int flags = 0;

#define store(ch)                                                                                  \
	if (--siz_left >= 0)                                                                           \
		*np++ = ch;

			if (ch == '.')
			{
				/*	An embarrasing ambiguity. We have either a
				    pp-number, a field operator, an ELLIPSIS or
				    an error (..).
				*/
				ch = GetChar();
				if (!is_dig(ch))
				{ /* . or ... */
					if (ch == '.')
					{
						if ((ch = GetChar()) == '.')
							return ptok->tk_symb = ELLIPSIS;
						UnGetChar(); /* not '.' */
						ChPushBack('.'); /* sigh ... */
					}
					else
						UnGetChar(); /* not '.' */
					return ptok->tk_symb = '.';
				}
				UnGetChar();
				ch = '.';
				flags |= FLG_DOTSEEN;
			}
			store(ch);
			ch = GetChar();
			while (in_idf(ch) || ch == '.')
			{
				store(ch);
				if (ch == '.')
					flags |= FLG_DOTSEEN;
				if (ch == 'e' || ch == 'E')
				{
					flags |= FLG_ESEEN;
					ch = GetChar();
					if (ch == '+' || ch == '-')
					{
						flags |= FLG_DOTSEEN; /* trick */
						store(ch);
						ch = GetChar();
					}
				}
				else
					ch = GetChar();
			}
			store('\0');
			UnGetChar();

			np = &buf[0];
			ch = *np++;
			if (siz_left < 0)
			{
				lexerror("number too long");
				if ((flags & FLG_DOTSEEN)
				    || (flags & FLG_ESEEN && !(ch == '0' && (*np == 'x' || *np == 'X'))))
				{
					ptok->tk_fval = Salloc("0.0", (unsigned)4);
					ptok->tk_fund = DOUBLE;
					return ptok->tk_symb = FLOATING;
				}
				ptok->tk_ival = 1;
				ptok->tk_fund = ULONG;
				ptok->tk_symb = INTEGER;
			}
			/* Now, the pp-number must be converted into a token */
			if ((flags & FLG_DOTSEEN)
			    || (flags & FLG_ESEEN && !(ch == '0' && (*np == 'x' || *np == 'X'))))
			{
				strflt2tok(&buf[0], ptok);
				return ptok->tk_symb = FLOATING;
			}
			strint2tok(&buf[0], ptok);
			return ptok->tk_symb = INTEGER;
		}
		case STEOI: /* end of text on source file	*/
			return ptok->tk_symb = EOI;
		default: /* this cannot happen	*/
			crash("bad class for char 0%o", ch);
	}
	/*NOTREACHED*/
}

static arith char_constant(char* nm)
{
	register arith val = 0;
	register int ch;
	int size = 0;

	ch = GetChar();
	if (ch == '\'')
		lexerror("%s constant too short", nm);
	else
		while (ch != '\'')
		{
			if (ch == '\n')
			{
				lexerror("newline in %s constant", nm);
				LineNumber++;
				break;
			}
			if (ch == '\\')
				ch = quoted(GetChar());
			if (ch >= 128)
				ch -= 256;
			if (size < (int)int_size)
				val |= ch << 8 * size;
			size++;
			ch = GetChar();
		}
	if (size > 1)
		lexstrict("%s constant includes more than one character", nm);
	if (size > (int)int_size)
		lexerror("%s constant too long", nm);
	return val;
}

static char* string_token(char *nm, int stop_char, int *plen)
{
	register int ch;
	register int str_size;
	register char* str = Malloc((unsigned)(str_size = ISTRSIZE));
	register int pos = 0;

	ch = GetChar();
	while (ch != stop_char)
	{
		if (ch == '\n')
		{
			lexerror("newline in %s", nm);
			LineNumber++;
			break;
		}
		if (ch == EOI)
		{
			lexerror("end-of-file inside %s", nm);
			break;
		}
		if (ch == '\\' && !AccFileSpecifier)
			ch = quoted(GetChar());
		str[pos++] = ch;
		if (pos == str_size)
			str = Realloc(str, (unsigned)(str_size += RSTRSIZE));
		ch = GetChar();
	}
	str[pos++] = '\0'; /* for filenames etc. */
	*plen = pos;
	return str;
}

static int quoted(register int ch)
{
	/*	quoted() replaces an escaped character sequence by the
	    character meant.
	*/
	/* first char after backslash already in ch */
	if (!is_oct(ch))
	{ /* a quoted char */
		switch (ch)
		{
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
			case 'a': /* alert */
				ch = '\007';
				break;
			case 'v': /* vertical tab */
				ch = '\013';
				break;
			case 'x': /* quoted hex */
			{
				register int hex = 0;
				register int vch;

				for (;;)
				{
					ch = GetChar();
					if ((vch = hex_val(ch)) == -1)
						break;
					hex = hex * 16 + vch;
				}
				UnGetChar();
				ch = hex;
			}
		}
	}
	else
	{ /* a quoted octal */
		register int oct = 0, cnt = 0;

		do
		{
			oct = oct * 8 + (ch - '0');
			ch = GetChar();
		} while (is_oct(ch) && ++cnt < 3);
		UnGetChar();
		ch = oct;
	}
	return ch & 0377;
}

static int hex_val(register int ch)
{
	return is_dig(ch) ? ch - '0' : is_hex(ch) ? (ch - 'a' + 10) & 017 : -1;
}

int GetChar(void)
{
	/*	The routines GetChar and trigraph parses the trigraph
	    sequences and removes occurences of \\\n.
	*/
	register int ch;

	LoadChar(ch);
	return (LexSave = ch);
}

/* strflt2tok only checks the syntax of the floating-point number and
 * selects the right type for the number.
 */
static void strflt2tok(char fltbuf[], struct token* ptok)
{
	register char* cp = fltbuf;
	int malformed = 0;

	while (is_dig(*cp))
		cp++;
	if (*cp == '.')
	{
		cp++;
		while (is_dig(*cp))
			cp++;
	}
	if (*cp == 'e' || *cp == 'E')
	{
		cp++;
		if (*cp == '+' || *cp == '-')
			cp++;
		if (!is_dig(*cp))
			malformed++;
		while (is_dig(*cp))
			cp++;
	}
	if (*cp == 'f' || *cp == 'F')
	{
		if (*(cp + 1))
			malformed++;
		*cp = '\0';
		ptok->tk_fund = FLOAT;
	}
	else if (*cp == 'l' || *cp == 'L')
	{
		if (*(cp + 1))
			malformed++;
		*cp = '\0';
		ptok->tk_fund = LNGDBL;
	}
	else
	{
		if (*cp)
			malformed++;
		ptok->tk_fund = DOUBLE;
	}
	if (malformed)
	{
		lexerror("malformed floating constant");
		ptok->tk_fval = Salloc("0.0", (unsigned)4);
	}
	else
	{
		ptok->tk_fval = Salloc(fltbuf, (unsigned)(cp - fltbuf + 1));
	}
}

static void strint2tok(char intbuf[], struct token* ptok)
{
	register char* cp = intbuf;
	int base = 10;
	arith val = 0, dig, ubound;
	int uns_flg = 0, lng_flg = 0, malformed = 0, ovfl = 0;
	int fund;

	assert(*cp != '-');
	if (*cp == '0')
	{
		cp++;
		if (*cp == 'x' || *cp == 'X')
		{
			cp++;
			base = 16;
		}
		else
			base = 8;
	}
	/* The upperbound will be the same as when computed with
	 * max_unsigned_arith / base (since base is even). The problem here
	 * is that unsigned arith is not accepted by all compilers.
	 */
	ubound = max_arith / (base / 2);

	while (is_hex(*cp))
	{
		dig = hex_val(*cp);
		if (dig >= base)
		{
			malformed++; /* ignore */
		}
		else
		{
			if (val < 0 || val > ubound)
				ovfl++;
			val *= base;
			if (val < 0 && val + dig >= 0)
				ovfl++;
			val += dig;
		}
		cp++;
	}

	while (*cp)
	{
		if (*cp == 'l' || *cp == 'L')
			lng_flg++;
		else if (*cp == 'u' || *cp == 'U')
			uns_flg++;
		else
			break;
		cp++;
	}
	if (*cp)
	{
		malformed++;
	}
	if (malformed)
	{
		lexerror(
		    "malformed %s integer constant",
		    (base == 10 ? "decimal" : (base == 8 ? "octal" : "hexadecimal")));
	}
	else
	{
		if (lng_flg > 1)
			lexerror("only one long suffix allowed");
		if (uns_flg > 1)
			lexerror("only one unsigned suffix allowed");
	}
	if (ovfl)
	{
		lexwarning("overflow in constant");
		fund = ULONG;
	}
	else if (!lng_flg && (val & full_mask[(int)int_size]) == val)
	{
		if (val >= 0 && val <= max_int)
		{
			fund = INT;
		}
		else if (int_size == long_size)
		{
			fund = UNSIGNED;
		}
		else if (base == 10 && !uns_flg)
			fund = LONG;
		else
			fund = UNSIGNED;
	}
	else if ((val & full_mask[(int)long_size]) == val)
	{
		if (val >= 0)
			fund = LONG;
		else
			fund = ULONG;
	}
	else
	{ /* sizeof(arith) is greater than long_size */
		assert(arith_size > long_size);
		lexwarning("constant too large for target machine");
		/* cut the size to prevent further complaints */
		val &= full_mask[(int)long_size];
		fund = ULONG;
	}
	if (lng_flg)
	{
		/* fund can't be INT */
		if (fund == UNSIGNED)
			fund = ULONG;
	}
	if (uns_flg)
	{
		if (fund == INT)
			fund = UNSIGNED;
		else if (fund == LONG)
			fund = ULONG;
	}
	ptok->tk_fund = fund;
	ptok->tk_ival = val;
}
