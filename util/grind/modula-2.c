/* $Id$ */

/* Language dependant support; this one is for Modula-2 */

#include <stdio.h>
#include <alloc.h>
#include <assert.h>

#include "position.h"
#include "class.h"
#include "langdep.h"
#include "Lpars.h"
#include "idf.h"
#include "token.h"
#include "expr.h"
#include "tree.h"
#include "operator.h"
#include "misc.h"

extern FILE *db_out, *db_in;

extern double
	atof();

static int
	print_string(),
	print_char(),
	get_number(),
	getname(),
	get_token(),
	getstring(),
	print_op(),
	binop_prio(),
	unop_prio(),
	fix_bin_to_pref();

static long
	array_elsize();

static struct langdep m2 = {
	1,

	"%ld",
	"%loB",
	"%lXH",
	"%lu",
	"%lXH",
	"%.14G",

	"[",
	"]",
	"(",
	")",
	"{",
	"}",

	print_string,
	print_char,
	array_elsize,
	binop_prio,
	unop_prio,
	getstring,
	getname,
	get_number,
	get_token,
	print_op,
	fix_bin_to_pref
};

struct langdep *m2_dep = &m2;

static
print_char(c)
  int	c;
{
  c &= 0377;
  fprintf(db_out, (c >= 040 && c < 0177) ? "'%c'" : "%oC", c);
}

static
print_string(f, s, len)
  FILE	*f;
  char	*s;
  int	len;
{
  register char	*str = s;
  int delim = '\'';

  while (*str) {
	if (*str++ == '\'') delim = '"';
  }
  fprintf(f, "%c%.*s%c", delim, len, s, delim);
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
unop_prio(op)
  int	op;
{
  switch(op) {
  case E_NOT:
  	return 5;
  case E_MIN:
  case E_PLUS:
	return 3;
  }
  return 1;
}

static int
binop_prio(op)
  int	op;
{
  switch(op) {
  case E_SELECT:
	return 9;
  case E_ARRAY:
	return 5;
  case E_AND:
  case E_MUL:
  case E_DIV:
  case E_MOD:
	return 4;

  case E_PLUS:
  case E_MIN:
  case E_OR:
	return 3;

  case E_IN:
  case E_EQUAL:
  case E_NOTEQUAL:
  case E_LTEQUAL:
  case E_GTEQUAL:
  case E_LT:
  case E_GT:
	return 2;
  }
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

  ch = getc(db_in);
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
getname(c)
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
  case '[':
	tok.ival = E_ARRAY;
	/* fall through */
  case '(':
  case ')':
  case ']':
  case '`':
  case '{':
  case '}':
  case ':':
  case ',':
  case '\\':
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
	error((c >= 040 && c < 0177) ? "%s'%c'" : "%s'\\0%o'", "illegal character ", c);
	return LLlex();
  }
}

static int 
getstring(c)
  int	c;
{
  register int ch;
  char buf[512];
  register int len = 0;

  while (ch = getc(db_in), ch != c) {
	if (ch == '\n') {
		error("newline in string");
		ungetc(ch, db_in);
		break;
	}
	buf[len++] = ch;
  }
  buf[len++] = 0;
  tok.str = Salloc(buf, (unsigned) len);
  return STRING;
}

static
print_op(f, p)
  FILE		*f;
  p_tree	p;
{
  switch(p->t_oper) {
  case OP_UNOP:
  	switch(p->t_whichoper) {
	case E_MIN:
		fputs("-", f);
		print_node(f, p->t_args[0], 0);
		break;
	case E_PLUS:
		fputs("+", f);
		print_node(f, p->t_args[0], 0);
		break;
	case E_NOT:
		fputs("~", f);
		print_node(f, p->t_args[0], 0);
		break;
	case E_DEREF:
		print_node(f, p->t_args[0], 0);
		fputs("^", f);
		break;
	}
	break;
  case OP_BINOP:
	if (p->t_whichoper == E_ARRAY) {
		print_node(f, p->t_args[0], 0);
		fputs("[", f);
		print_node(f, p->t_args[1], 0);
		fputs("]", f);
		break;
	}
	if (p->t_whichoper == E_SELECT) {
		print_node(f, p->t_args[0], 0);
		fputs(".", f);
		print_node(f, p->t_args[1], 0);
		break;
	}
	fputs("(", f);
	print_node(f, p->t_args[0], 0);
	switch(p->t_whichoper) {
	case E_AND:
		fputs("&", f);
		break;
	case E_OR:
		fputs("|", f);
		break;
	case E_DIV:
		fputs("/", f);
		break;
	case E_MOD:
		fputs(" MOD ", f);
		break;
	case E_IN:
		fputs(" IN ", f);
		break;
	case E_PLUS:
		fputs("+", f);
		break;
	case E_MIN:
		fputs("-", f);
		break;
	case E_MUL:
		fputs("*", f);
		break;
	case E_EQUAL:
		fputs("=", f);
		break;
	case E_NOTEQUAL:
		fputs("#", f);
		break;
	case E_LTEQUAL:
		fputs("<=", f);
		break;
	case E_GTEQUAL:
		fputs(">=", f);
		break;
	case E_LT:
		fputs("<", f);
		break;
	case E_GT:
		fputs(">", f);
		break;
	}
	print_node(f, p->t_args[1], 0);
	fputs(")", f);
	break;
  }
}

static
fix_bin_to_pref()
{
  /* No problems of this kind in Modula-2 */
}
