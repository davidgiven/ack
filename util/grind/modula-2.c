/* $Header$ */

/* Language dependant support; this one is for Modula-2 */

#include <stdio.h>

#include "class.h"
#include "langdep.h"
#include "Lpars.h"
#include "idf.h"
#include "token.h"
#include "expr.h"

extern FILE *db_out, *db_in;

extern int
	get_string();

extern double
	atof();

static int
	print_string(),
	get_number(),
	get_name(),
	get_token(),
	op_prio();

static long
	array_elsize();

static struct langdep m2 = {
	"%ld",
	"%loB",
	"%lXH",
	"%lu",
	"%lXH",
	"%g",
	"%oC",

	"[",
	"]",
	"(",
	")",
	"{",
	"}",

	print_string,
	array_elsize,
	op_prio,
	get_string,
	get_name,
	get_number,
	get_token
};

struct langdep *m2_dep = &m2;

static int
print_string(s)
  char	*s;
{
  register char	*str = s;
  int delim = '\'';

  while (*str) {
	if (*str++ == '\'') delim = '"';
  }
  fprintf(db_out, "%c%s%c", delim, s, delim);
}

extern long	int_size;

static long
array_elsize(size)
  long	size;
{
  if (! (int_size % size)) return size;
  if (! (size % int_size)) return size;
  return ((size + int_size - 1) / int_size) * int_size;
}

static int
op_prio(op)
  int	op;
{
  /* ??? to be written ??? */
  return 1;
}

static int
get_number(ch)
  register int	ch;
{
  /*	The problem arising with the "parsing" of a number
	is that we don't know the base in advance so we
	have to read the number with the help of a rather
	complex finite automaton.
  */
  enum statetp {Oct,Hex,Dec,OctEndOrHex,End,Real};
  register enum statetp state;
  char buf[512+1];
  register int base = 10;
  register char *np = &buf[0];

  *np++ = ch;
  state = is_oct(ch) ? Oct : Dec;
  ch = getc(db_in);
  for (;;) {
	switch(state) {
	case Oct:
		while (is_oct(ch))	{
			if (np < &buf[512]) *np++ = ch;
			ch = getc(db_in);
		}
		if (ch == 'B' || ch == 'C') {
			state = OctEndOrHex;
			break;
		}
		/* Fall Through */
	case Dec:
		base = 10;
		while (is_dig(ch))	{
			if (np < &buf[512]) {
				*np++ = ch;
			}
			ch = getc(db_in);
		}
		if (is_hex(ch)) state = Hex;
		else if (ch == '.') state = Real;
		else {
			state = End;
			if (ch == 'H') base = 16;
			else ungetc(ch, db_in);
		}
		break;

	case Hex:
		while (is_hex(ch))	{
			if (np < &buf[512]) *np++ = ch;
			ch = getc(db_in);
		}
		base = 16;
		state = End;
		if (ch != 'H') {
			error("H expected after hex number");
			ungetc(ch, db_in);
		}
		break;

	case OctEndOrHex:
		if (np < &buf[512]) *np++ = ch;
		ch = getc(db_in);
		if (ch == 'H') {
			base = 16;
			state = End;
			break;
		}
		if (is_hex(ch)) {
			state = Hex;
			break;
		}
		ungetc(ch, db_in);
		ch = *--np;
		*np++ = '\0';
		/* Fall through */
		
	case End:
		*np = '\0';
		if (np >= &buf[512]) {
			tok.ival = 1;
			error("constant too long");
		}
		else {
			np = &buf[0];
			while (*np == '0') np++;
			tok.ival = 0;
			while (*np) {
				int c;

				if (is_dig(*np)) {
					c = *np++ - '0';
				}
				else {
					c = *np++ - 'A' + 10;
				}
				tok.ival *= base;
				tok.ival += c;
			}
		}
		return INTEGER;
	}
	if (state == Real) break;
  }

  /* a real real constant */
  if (np < &buf[512]) *np++ = '.';

  while (is_dig(ch)) {
	/* 	Fractional part
	*/
	if (np < &buf[512]) *np++ = ch;
	ch = getc(db_in);
  }

  if (ch == 'E') {
	/*	Scale factor
	*/
	if (np < &buf[512]) *np++ = ch;
	ch = getc(db_in);
	if (ch == '+' || ch == '-') {
		/*	Signed scalefactor
		*/
		if (np < &buf[512]) *np++ = ch;
		ch = getc(db_in);
	}
	if (is_dig(ch)) {
		do {
			if (np < &buf[512]) *np++ = ch;
			ch = getc(db_in);
		} while (is_dig(ch));
	}
	else {
		error("bad scale factor");
	}
  }

  *np++ = '\0';
  ungetc(ch, db_in);

  if (np >= &buf[512]) {
	tok.fval = 0.0;
	error("real constant too long");
  }
  else	tok.fval = atof(buf);
  return REAL;
}

static int
get_name(c)
  register int	c;
{
  char	buf[512+1];
  register char	*p = &buf[0];
  register struct idf *id;

  do {
	if (p - buf < 512) *p++ = c;
	c = getc(db_in);
  } while (in_idf(c));
  ungetc(c, db_in);
  *p = 0;
  /* now recognize AND, DIV, IN, MOD, NOT, OR */
  switch(buf[0]) {
  case 'A':
	if (strcmp(buf, "AND") == 0) {
		tok.ival = E_AND;
		return BIN_OP;
	}
	break;
  case 'D':
	if (strcmp(buf, "DIV") == 0) {
		tok.ival = E_DIV;
		return BIN_OP;
	}
	break;
  case 'I':
	if (strcmp(buf, "IN") == 0) {
		tok.ival = E_IN;
		return BIN_OP;
	}
	break;
  case 'M':
	if (strcmp(buf, "MOD") == 0) {
		tok.ival = E_MOD;
		return BIN_OP;
	}
	break;
  case 'N':
	if (strcmp(buf, "NOT") == 0) {
		tok.ival = E_NOT;
		return PREF_OP;
	}
	break;
  case 'O':
	if (strcmp(buf, "OR") == 0) {
		tok.ival = E_OR;
		return BIN_OP;
	}
	break;
  }
  id = str2idf(buf, 1);
  tok.idf = id;
  tok.str = id->id_text;
  return id->id_reserved ? id->id_reserved : NAME;
}

static int
get_token(c)
  register int	c;
{
  switch(c) {
  case '(':
  case ')':
  case '[':
  case ']':
  case '`':
  case '{':
  case '}':
  case ':':
  case ',':
	return c;

  case '.':
	tok.ival = E_SELECT;
	return SEL_OP;
  case '+':
	tok.ival = E_PLUS;
	return PREF_OR_BIN_OP;
  case '-':
	tok.ival = E_MIN;
	return PREF_OR_BIN_OP;
  case '*':
	tok.ival = E_MUL;
	return BIN_OP;
  case '/':
	tok.ival = E_DIV;
	return BIN_OP;
  case '&':
	tok.ival = E_AND;
	return BIN_OP;
  case '|':
	tok.ival = E_OR;
	return BIN_OP;
  case '=':
	tok.ival = E_EQUAL;
	return BIN_OP;
  case '#':
	tok.ival = E_NOTEQUAL;
	return BIN_OP;
  case '<':
	c = getc(db_in);
	if (c == '>') {
		tok.ival = E_NOTEQUAL;
		return BIN_OP;
	}
	if (c == '=') {
		tok.ival = E_LTEQUAL;
		return BIN_OP;
	}
	ungetc(c, db_in);
	tok.ival = E_LT;
	return BIN_OP;
  case '>':
	c = getc(db_in);
	if (c == '=') {
		tok.ival = E_GTEQUAL;
		return BIN_OP;
	}
	ungetc(c, db_in);
	tok.ival = E_GT;
	return BIN_OP;
  case '^':
	tok.ival = E_DEREF;
	return POST_OP;
  case '~':
	tok.ival = E_NOT;
	return PREF_OP;
  default:
	error("illegal character 0%o", c);
	return LLlex();
  }
}
