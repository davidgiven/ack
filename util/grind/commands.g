/* $Header$ */

/* Command grammar */
{
#include	<stdio.h>
#include	<alloc.h>
#include	<setjmp.h>
#include	<signal.h>

#include	"ops.h"
#include	"class.h"
#include	"position.h"
#include	"file.h"
#include	"idf.h"
#include	"symbol.h"
#include	"tree.h"
#include	"langdep.h"
#include	"token.h"

extern char	*Salloc();
extern t_lineno	currline;
extern FILE	*db_in;

int		errorgiven;
static int	extended_charset = 0;
static int	in_expression = 0;
jmp_buf		jmpbuf;

static int	init_del();
static int	skip_to_eol();

struct token	tok, aside;

#define prio(op)	((*(currlang->op_prio))(op))
}
%start Commands, commands;

%lexical LLlex;

commands
  { p_tree com, lastcom = 0;
  }
:
			{ if (! setjmp(jmpbuf)) {
				init_del();
			  }
			  else {
				skip_to_eol();
				goto prmpt;
			  }
			}
  [ %persistent command_line(&com)
			{ if (com) {
				if (errorgiven) {
					freenode(com);
					com = 0;
				}
				if (lastcom && !in_status(lastcom) &&
				    lastcom != run_command) {
					freenode(lastcom);
					lastcom = 0;
				}

				if (com) {
			  		if (repeatable(com)) {
						lastcom = com;
					}
					eval(com);
					if (! repeatable(com) &&
					    ! in_status(com) &&
					    com != run_command) {
						freenode(com);
					}
				}
			  } else if (lastcom && ! errorgiven) eval(lastcom);
			}
    [	'\n' 		{ prmpt: prompt(); }
    |	';'
    ]			{ errorgiven = 0; }
  ]*
			{ signal_child(SIGKILL); }
;

command_line(p_tree *p;)
:
  list_command(p)
| file_command(p)
| run_command(p)
| stop_command(p)
| when_command(p)
| continue_command(p)
| step_command(p)
| next_command(p)
| regs_command(p)
| WHERE			{ *p = mknode(OP_WHERE); }
| STATUS		{ *p = mknode(OP_STATUS); }
| DUMP			{ *p = mknode(OP_DUMP); }
| RESTORE INTEGER	{ *p = mknode(OP_RESTORE, tok.ival); }
| delete_command(p)
| print_command(p)
| trace_command(p)
|			{ *p = 0; }
;

list_command(p_tree *p;)
  { p_tree t1 = 0, t2 = 0; }
:
  LIST
  [
  | lin_num(&t1)
    [ ',' lin_num(&t2)
    |			{ t2 = mknode(OP_INTEGER, t1->t_ival); }
    ]
  ]			{ *p = mknode(OP_LIST, t1, t2); }
;

file_command(p_tree *p;)
:
  XFILE			{ extended_charset = 1; }
  [			{ *p = 0; }
  | name(p)		{ (*p)->t_idf = str2idf((*p)->t_str, 0); }
  ]			{ *p = mknode(OP_FILE, *p);
			  extended_charset = 0;
			}
;

run_command(p_tree *p;)
:
  RUN			{ extended_charset = 1; *p = 0; }
  args(p)		{ *p = mknode(OP_RUN, *p);
			  extended_charset = 0;
			  freenode(run_command);
			  run_command = *p;
			}
| RERUN			{ if (! run_command) {
				error("no run command given yet");
			  }
			  else *p = run_command;
			}
;

stop_command(p_tree *p;)
  { p_tree whr = 0, cond = 0; }
:
  STOP
  where(&whr)?
  condition(&cond)?	{ if (! whr && ! cond) {
				error("no position or condition");
				*p = 0;
			  }
			  else *p = mknode(OP_STOP, whr, cond);
			}
;

trace_command(p_tree *p;)
  { p_tree whr = 0, cond = 0, exp = 0; }
:
  TRACE
  [ ON expression(&exp, 1) ]?
  where(&whr)?
  condition(&cond)?	{ *p = mknode(OP_TRACE, whr, cond, exp); }
;

continue_command(p_tree *p;)
  { long l; p_tree pos = 0; }
:
  CONT
  [ INTEGER		{ l = tok.ival; }
  |			{ l = 1; }
  ]
  position(&pos)?
  			{ *p = mknode(OP_CONT, mknode(OP_INTEGER, l), pos); }
;

when_command(p_tree *p;)
  { p_tree	whr = 0, cond = 0; }
:
  WHEN
  where(&whr)?
  condition(&cond)?
  '{' 
  command_line(p)
  [ ';'			{ *p = mknode(OP_LINK, *p, (p_tree) 0);
			  p = &((*p)->t_args[1]);
			}
    command_line(p)
  ]*
  '}'
			{ if (! whr && ! cond) {
				error("no position or condition");
				freenode(*p);
				*p = 0;
			  }
			  else *p = mknode(OP_WHEN, whr, cond, *p);
			}
;

step_command(p_tree *p;)
  { long	l; }
:
  STEP
  [ INTEGER		{ l = tok.ival; }
  |			{ l = 1; }
  ]			{ *p = mknode(OP_STEP, l); }
;

next_command(p_tree *p;)
  { long	l; }
:
  NEXT
  [ INTEGER		{ l = tok.ival; }
  |			{ l = 1; }
  ]			{ *p = mknode(OP_NEXT, l); }
;

regs_command(p_tree *p;)
  { long	l; }
:
  REGS
  [ INTEGER		{ l = tok.ival; }
  |			{ l = 0; }
  ]			{ *p = mknode(OP_REGS, l); }
;

delete_command(p_tree *p;)
:
  DELETE
  INTEGER		{ *p = mknode(OP_DELETE, tok.ival); }
;

print_command(p_tree *p;)
:
  PRINT expression(p, 1){ *p = mknode(OP_PRINT, *p); 
			  p = &((*p)->t_args[0]);
			}
  [ ','			{ *p = mknode(OP_LINK, *p, (p_tree) 0);
			  p = &((*p)->t_args[1]);
			}
    expression(p, 1)
  ]*
;

condition(p_tree *p;)
:
  IF expression(p, 1)
;

where(p_tree *p;)
:
  IN qualified_name(p)	{ *p = mknode(OP_IN, *p); }
|
  position(p)
;

expression(p_tree *p; int level;)
  { int currprio, currop; }
:			{ in_expression++; }
  factor(p)
  [ %while ((currprio = prio(currop = (int) tok.ival)) > level)
	[ BIN_OP | PREF_OR_BIN_OP ] 
			{ *p = mknode(OP_BINOP, *p, (p_tree) 0);
			  (*p)->t_whichoper = currop;
			}
	expression(&((*p)->t_args[1]), currprio)
  ]*
			{ in_expression--; }
;

factor(p_tree *p;)
:
  '(' expression(p, 1) ')'
|
  INTEGER		{ *p = mknode(OP_INTEGER, tok.ival); }
|
  REAL			{ *p = mknode(OP_REAL, tok.fval); }
|
  STRING		{ *p = mknode(OP_STRING, tok.str); }
|
  designator(p)
|
  PREF_OP		{ *p = mknode(OP_UNOP, (p_tree) 0);
			  (*p)->t_whichoper = (int) tok.ival;
			}
  factor(&(*p)->t_args[0])
;

designator(p_tree *p;)
:
  qualified_name(p)
  [
	SEL_OP		{ *p = mknode(OP_BINOP, *p, (p_tree) 0);
			  (*p)->t_whichoper = (int) tok.ival;
			}
	name(&(*p)->t_args[1])
  |
	'['		{ *p = mknode(OP_BINOP, *p, (p_tree) 0);
			  (*p)->t_whichoper = '[';
			}
	expression(&(*p)->t_args[1], 1)
	']'
  |
	POST_OP		{ *p = mknode(OP_UNOP, *p);
			  (*p)->t_whichoper = (int) tok.ival;
			}
  ]*
;

position(p_tree *p;)
  { p_tree lin;
    char *str;
  }
:
  AT
  [ STRING		{ str = tok.str; }
    ':'
  |			{ if (! currfile) str = 0;
			  else str = currfile->sy_idf->id_text;
			}
  ]
  lin_num(&lin)		{ *p = mknode(OP_AT, lin->t_ival, str);
			  freenode(lin);
			}
;

args(p_tree *p;)
  { int first_time = 1; }
:
  [			{ if (! first_time) {
				*p = mknode(OP_LINK, *p, (p_tree) 0);
				p = &((*p)->t_args[1]);
			  }
			  first_time = 0;
			}
	arg(p)
  ]*
;

arg(p_tree *p;)
:
  name(p)
|
  '>' name(p)		{ (*p)->t_oper = OP_OUTPUT; }
|
  '<' name(p)		{ (*p)->t_oper = OP_INPUT; }
;

lin_num(p_tree *p;)
:
  INTEGER		{ *p = mknode(OP_INTEGER, tok.ival); }
;

qualified_name(p_tree *p;)
:
  name(p)
  [	'`'		{ *p = mknode(OP_SELECT, *p, (p_tree) 0); }
	name(&((*p)->t_args[1]))
  ]*
;

name(p_tree *p;)
:
  [ XFILE
  | LIST
  | RUN
  | RERUN
  | STOP
  | WHEN
  | AT
  | IN
  | IF
  | NAME
  | CONT
  | STEP
  | NEXT
  | REGS
  | WHERE
  | STATUS
  | PRINT
  | DELETE
  | DUMP
  | RESTORE
  | TRACE
  | ON
  ]			{ *p = mknode(OP_NAME, tok.idf, tok.str); }
;

{
int
LLlex()
{
  register int c;

  if (ASIDE) {
	tok = aside;
	ASIDE = 0;
	return TOK;
  }
  do {
	c = getc(db_in);
  } while (c != EOF && class(c) == STSKIP);
  if (c == EOF) return c;
  switch(class(c)) {
  case STSTR:
	TOK = (*currlang->get_string)(c);
	break;
  case STIDF:
	if (in_expression) TOK = (*currlang->get_name)(c);
	else TOK = get_name(c);
	break;
  case STDOT:
	c = getc(db_in);
	if (c == EOF || class(c) != STNUM) {
		ungetc(c,db_in);
		TOK = '.';
		break;
	}
	/* Fall through */
  case STNUM:
	TOK = (*currlang->get_number)(c);
	break;
  case STNL:
	TOK = c;
	break;
  case STSIMP:
	if (! in_expression) {
		TOK = c;
		break;
	}
	/* Fall through */
  default:
	TOK = (*currlang->get_token)(c);
	break;
  }
  return TOK;
}

int
get_name(c)
  register int	c;
{
  char	buf[512+1];
  register char	*p = &buf[0];
  register struct idf *id;

  do {
	if (p - buf < 512) *p++ = c;
	c = getc(db_in);
  } while ((extended_charset && in_ext(c)) || in_idf(c));
  ungetc(c, db_in);
  *p = 0;
  if (extended_charset) {
	tok.idf = 0;
	tok.str = Salloc(buf, (unsigned) (p - buf));
	return NAME;
  }
  id = str2idf(buf, 1);
  tok.idf = id;
  tok.str = id->id_text;
  return id->id_reserved ? id->id_reserved : NAME;
}

static int
quoted(ch)
  int	ch;
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
	}
  }
  else {				/* a quoted octal */
	register int oct = 0, cnt = 0;

	do {
		oct = oct*8 + (ch-'0');
		ch = getc(db_in);
	} while (is_oct(ch) && ++cnt < 3);
	ungetc(ch, db_in);
	ch = oct;
  }
  return ch&0377;

}

int 
get_string(c)
  int	c;
{
  register int ch;
  char buf[512];
  register int len = 0;

  while (ch = getc(db_in), ch != c) {
	if (ch == '\n') {
		error("newline in string");
		break;
	}
	if (ch == '\\') {
		ch = getc(db_in);
		ch = quoted(ch);
	}
	buf[len++] = ch;
  }
  buf[len++] = 0;
  tok.str = Salloc(buf, (unsigned) len);
  return STRING;
}

static int
val_in_base(c, base)
  register int c;
{
  return is_dig(c) 
	? c - '0'
	: base != 16
	  ? -1
	  : is_hex(c)
	    ? (c - 'a' + 10) & 017
	    : -1;
}

int
get_number(c)
  register int	c;
{
  char buf[512+1];
  register int base = 10;
  register char *p = &buf[0];
  register long val = 0;
  register int val_c;

  if (c == '0') {
	/* check if next char is an 'x' or an 'X' */
	c = getc(db_in);
	if (c == 'x' || c == 'X') {
		base = 16;
		c = getc(db_in);
	}
	else	base = 8;
  }
  while (val_c = val_in_base(c, base), val_c >= 0) {
	val = val * base + val_c;
	if (p - buf < 512) *p++ = c;
	c = getc(db_in);
  }
  if (base == 16 || !((c == '.' || c == 'e' || c == 'E'))) {
	ungetc(c, db_in);
	tok.ival = val;
	return INTEGER;
  }
  if (c == '.') {
	if (p - buf < 512) *p++ = c;
	c = getc(db_in);
  }
  while (is_dig(c)) {
	if (p - buf < 512) *p++ = c;
	c = getc(db_in);
  }
  if (c == 'e' || c == 'E') {
	if (p - buf < 512) *p++ = c;
	c = getc(db_in);
	if (c == '+' || c == '-') {
		if (p - buf < 512) *p++ = c;
		c = getc(db_in);
	}
	if (! is_dig(c)) {
		error("malformed floating constant");
	}
	while (is_dig(c)) {
		if (p - buf < 512) *p++ = c;
		c = getc(db_in);
	}
  }
  ungetc(c, db_in);
  *p++ = 0;
  if (p == &buf[512+1]) {
	error("floating point constant too long");
  }
  return REAL;
}

extern char * symbol2str();

LLmessage(t)
{
  if (t > 0) {
  	if (! errorgiven) {
		error("%s missing before %s", symbol2str(t), symbol2str(TOK));
	}
	aside = tok;
  }
  else if (t == 0) {
  	if (! errorgiven) {
		error("%s unexpected", symbol2str(TOK));
	}
  }
  else if (! errorgiven) {
	error("EOF expected");
  }
  errorgiven = 1;
}

static int
catch_del()
{
  signal(SIGINT, catch_del);
  signal_child(SIGEMT);
  longjmp(jmpbuf, 1);
}

static int
init_del()
{
  signal(SIGINT, catch_del);
}

static int
skip_to_eol()
{
  while (TOK != '\n' && TOK > 0) LLlex();
  wait_for_child("interrupted");
}
}
