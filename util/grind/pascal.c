/* $Id$ */

/* Language dependant support; this one is for Pascal */

#include <stdio.h>
#include <alloc.h>
#include <assert.h>
#include <ctype.h>

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

extern long
	atol();

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

static struct langdep pascal = {
	1,

	"%ld",
	"0%lo",
	"0x%lx",
	"%lu",
	"0x%lx",
	"%.14g",

	"[",
	"]",
	"(",
	")",
	"[",
	"]",

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

struct langdep *pascal_dep = &pascal;

static
print_char(c)
  int	c;
{
  c &= 0377;
  fprintf(db_out, (c >= 040 && c < 0177) ? "'%c'" : "chr(%d)", c);
}

static
print_string(f, s, len)
  FILE	*f;
  char	*s;
  int	len;
{
  register char	*str = s;

  putc('\'', f);
  while (*str && len > 0) {
	putc(*str, f);
	if (*str++ == '\'') putc('\'', f);
	len--;
  }
  putc('\'', f);
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
  	return 8;
  case E_MIN:
  case E_PLUS:
	return 6;
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
	return 9;
  case E_AND:
  case E_MUL:
  case E_DIV:
  case E_MOD:
	return 7;

  case E_PLUS:
  case E_MIN:
  case E_OR:
	return 6;

  case E_IN:
  case E_EQUAL:
  case E_NOTEQUAL:
  case E_LTEQUAL:
  case E_GTEQUAL:
  case E_LT:
  case E_GT:
	return 5;
  }
  return 1;
}

static int
get_number(ch)
  register int	ch;
{
  char buf[512+1];
  register char *np = &buf[0];
  int real_mode = 0;

  while (is_dig(ch))	{
	if (np < &buf[512]) *np++ = ch;
	ch = getc(db_in);
  }

  if (ch == '.') {
	real_mode = 1;
  	if (np < &buf[512]) *np++ = '.';
  	ch = getc(db_in);
  	while (is_dig(ch)) {
		/* 	Fractional part
		*/
		if (np < &buf[512]) *np++ = ch;
		ch = getc(db_in);
  	}
  }

  if (ch == 'E' || ch == 'e') {
	/*	Scale factor
	*/
	real_mode = 1;
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
  	if (! real_mode) {
		tok.ival = 0;
		error("constant too long");
  	}
	else {
		tok.fval = 0.0;
		error("real constant too long");
	}
  }
  else if (! real_mode) {
	tok.ival = atol(buf);
	return INTEGER;
  }
  tok.fval = atof(buf);
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
	if (isupper(c)) c = tolower(c);
	if (p - buf < 512) *p++ = c;
	c = getc(db_in);
  } while (in_idf(c));
  ungetc(c, db_in);
  *p = 0;
  /* now recognize and, div, in, mod, not, or */
  switch(buf[0]) {
  case 'a':
	if (strcmp(buf, "and") == 0) {
		tok.ival = E_AND;
		return BIN_OP;
	}
	break;
  case 'd':
	if (strcmp(buf, "div") == 0) {
		tok.ival = E_DIV;
		return BIN_OP;
	}
	break;
  case 'i':
	if (strcmp(buf, "in") == 0) {
		tok.ival = E_IN;
		return BIN_OP;
	}
	break;
  case 'm':
	if (strcmp(buf, "mod") == 0) {
		tok.ival = E_MOD;
		return BIN_OP;
	}
	break;
  case 'n':
	if (strcmp(buf, "not") == 0) {
		tok.ival = E_NOT;
		return PREF_OP;
	}
	break;
  case 'o':
	if (strcmp(buf, "or") == 0) {
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
  case '=':
	tok.ival = E_EQUAL;
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

  for (;;) {
	ch = getc(db_in);
	if (ch == c) {
		ch = getc(db_in);
		if (ch != c) {
			ungetc(ch, db_in);
			break;
		}
	}
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
		fputs(" not ", f);
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
		fputs(" and ", f);
		break;
	case E_OR:
		fputs(" or ", f);
		break;
	case E_DIV:
		fputs("/", f);
		break;
	case E_MOD:
		fputs(" mod ", f);
		break;
	case E_IN:
		fputs(" in ", f);
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
		fputs("<>", f);
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
  /* No problems of this kind in Pascal */
}
