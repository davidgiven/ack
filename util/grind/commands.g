/* $Header$ */

/* Command grammar */
{
#include	<stdio.h>
#include	<alloc.h>
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
#include	"expr.h"
#include	"misc.h"

extern char	*Salloc();
extern char	*strindex();
extern char	*strcpy();
extern void	signal_child();
extern FILE	*db_in;
extern int	disable_intr;
extern p_tree	run_command, print_command;

struct token	tok, aside;
int		errorgiven = 0;
int		child_interrupted = 0;
int		interrupted = 0;
int		eof_seen = 0;

static int	shellescape();

static int	extended_charset = 0;
static int	in_expression = 0;

#define binprio(op)	((*(currlang->binop_prio))(op))
#define unprio(op)	((*(currlang->unop_prio))(op))
}
%start Commands, commands;

%lexical LLlex;

commands
  { p_tree com, lastcom = 0;
    int give_prompt;
  }
:
			{ errorgiven = 0; }
  [ %persistent command_line(&com)
    [	'\n'		{ give_prompt = 1; }
    |	%default ';'	{ give_prompt = 0; }
    ]
			{ if (com) {
				if (lastcom) {
					freenode(lastcom);
					lastcom = 0;
				}
				if (errorgiven) {
					if (com != run_command) freenode(com);
					com = 0;
				}
				else {
					enterlog(com);
					eval(com);
			  		if (repeatable(com)) {
						lastcom = com;
					}
					else if (! in_status(com) &&
					        com != run_command &&
						com != print_command) {
						freenode(com);
						com = 0;
					}
				}
			  } else if (lastcom && ! errorgiven) {
				enterlog(lastcom);
				eval(lastcom);
			  }
			  if (give_prompt) {
			  	errorgiven = 0;
				interrupted = 0;
				prompt();
			  }
			}
  ]*
;

command_line(p_tree *p;)
:
			{ *p = 0; }
[
  list_command(p)
| file_command(p)
| run_command(p)
| stop_command(p)
| when_command(p)
| continue_command(p)
| step_command(p)
| next_command(p)
| regs_command(p)
| where_command(p)
| STATUS		{ *p = mknode(OP_STATUS); }
| DUMP			{ *p = mknode(OP_DUMP); }
| RESTORE opt_num(p)	{ *p = mknode(OP_RESTORE, *p); }
| delete_command(p)
| print_command(p)
| display_command(p)
| trace_command(p)
| set_command(p)
| help_command(p)
| FIND qualified_name(p){ *p = mknode(OP_FIND, *p); }
| WHICH qualified_name(p){ *p = mknode(OP_WHICH, *p); }
| able_command(p)
| '!'			{ (void) shellescape();
			  *p = mknode(OP_SHELL);
			}
| source_command(p)
| log_command(p)
| frame_command(p)
|
]
;

frame_command(p_tree	*p;)
:
  FRAME 
  [			{ *p = mknode(OP_FRAME, (p_tree) 0); }
  | count(p)		{ *p = mknode(OP_FRAME, *p); }
  | '-' count(p)	{ *p = mknode(OP_DOWN, *p); }
  | '+' count(p)	{ *p = mknode(OP_UP, *p); }
  ]
;

source_command(p_tree *p;)
:
  SOURCE		{ extended_charset = 1; }
  name(p)		{ (*p)->t_idf = str2idf((*p)->t_str, 0); }
  			{ *p = mknode(OP_SOURCE, *p);
			  extended_charset = 0;
			}
;

log_command(p_tree *p;)
:
  LOG			{ extended_charset = 1; }
  [ name(p)		{ (*p)->t_idf = str2idf((*p)->t_str, 0); }
  |			{ *p = 0; }
  ]
  			{ *p = mknode(OP_LOG, *p);
			  extended_charset = 0;
			}
;

where_command(p_tree *p;)
:
  WHERE opt_num(p)	{ *p = mknode(OP_WHERE, *p); }
;

list_command(p_tree *p;)
  { p_tree t1 = 0, t2 = 0; }
:
  LIST
  [
  | position(&t1)
  | qualified_name(&t1)
  ]
  [ ',' count(&t2)
  | '-' 
    [	count(&t2)	{ t2->t_ival = - t2->t_ival; }
    |			{ t2 = mknode(OP_INTEGER, -100000000L); }
    ]
  |
  ]
			{ *p = mknode(OP_LIST, t1, t2); }
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

help_command(p_tree *p;)
:
  [ HELP | '?' ]
  			{ *p = mknode(OP_HELP, (p_tree) 0); }
  [ name(&(*p)->t_args[0])?
  | '?'			{ (*p)->t_args[0] = mknode(OP_NAME, str2idf("help",0), (char *) 0); }
  | '!'			{ (*p)->t_args[0] = mknode(OP_NAME, (struct idf *) 0, "!"); }
  ]
;

run_command(p_tree *p;)
:
  RUN			{ extended_charset = 1; }
  args(p)		{ *p = mknode(OP_RUN, *p);
			  extended_charset = 0;
			}
| RERUN			{ if (! run_command) {
				error("no run command given yet");
			  }
			  else *p = run_command;
			}
  [ '?'			{ *p = mknode(OP_PRCOMM, *p); }
  |
  ]
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
  [ ON format_expression(&exp) ]?
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
  [ AT position(&pos) ]?
  			{ *p = mknode(OP_CONT, mknode(OP_INTEGER, l), pos); }
;

when_command(p_tree *p;)
  { p_tree	whr = 0, cond = 0; }
:
  WHEN
  where(&whr)?
  condition(&cond)?	{ *p = mknode(OP_WHEN, whr, cond, (p_tree) 0); 
			  p = &(*p)->t_args[2];
			}
  '{' 
  command_line(p)
  [ ';'			{ if (*p) {
				*p = mknode(OP_LINK, *p, (p_tree) 0);
			  	p = &((*p)->t_args[1]);
			  }
			}
    command_line(p)
  ]*
  '}'
			{ if (! whr && ! cond) {
				error("no position or condition");
			  }
			  else if (! *p) {
				error("no commands given");
			  }
			}
;

step_command(p_tree *p;)
:
  STEP			{ *p = mknode(OP_STEP, (p_tree) 0); }
  count(&(*p)->t_args[0])?
;

next_command(p_tree *p;)
:
  NEXT			{ *p = mknode(OP_NEXT, (p_tree) 0); }
  count(&(*p)->t_args[0])?
;

regs_command(p_tree *p;)
:
  REGS			{ *p = mknode(OP_REGS, (p_tree) 0); }
  count(&(*p)->t_args[0])?
;

delete_command(p_tree *p;)
:
  DELETE num_list(p)?	{ *p = mknode(OP_DELETE, *p); }
;

print_command(p_tree *p;)
:
  PRINT 
  [ format_expression_list(p)
			{ *p = mknode(OP_PRINT, *p); }
  |
			{ *p = mknode(OP_PRINT, (p_tree) 0); }
  ]
;

display_command(p_tree *p;)
:
  DISPLAY format_expression_list(p)
			{ *p = mknode(OP_DISPLAY, *p); }
;

format_expression_list(p_tree *p;)
:
  format_expression(p)
  [ ','			{ *p = mknode(OP_LINK, *p, (p_tree) 0);
			  p = &((*p)->t_args[1]);
			}
    format_expression(p)
  ]*
;

format_expression(p_tree *p;)
  { p_tree	p1; }
:
  expression(p, 0)
  [ '\\' name(&p1)	{ register char *c = p1->t_str;
			  while (*c) {
				if (! strindex("doshcax", *c)) {
					error("illegal format: %c", *c);
					break;
				}
				c++;
			  }
			  *p = mknode(OP_FORMAT, *p, p1);
			}
  |
  ]
;

set_command(p_tree *p;)
:
  SET expression(p, 0)	{ *p = mknode(OP_SET, *p, (p_tree) 0); }
  TO expression(&((*p)->t_args[1]), 0)
;

able_command(p_tree *p;)
:
  [ ENABLE 		{ *p = mknode(OP_ENABLE, (p_tree) 0); }
  | DISABLE 		{ *p = mknode(OP_DISABLE, (p_tree) 0); }
  ]
  num_list(&(*p)->t_args[0])?
;

num_list(p_tree *p;)
:
  num(p)
  [ ','			{ *p = mknode(OP_LINK, *p, (p_tree) 0); }
    num(&(*p)->t_args[1])
  ]*
;

condition(p_tree *p;)
:
  IF expression(p, 0)
;

where(p_tree *p;)
:
  IN qualified_name(p)	{ *p = mknode(OP_IN, *p, (p_tree) 0); }
  [ AT position(&((*p)->t_args[1])) ]?
|
  AT position(p)
;

expression(p_tree *p; int level;)
  { int currprio, currop; }
:			{ in_expression++; }
  factor(p)
  [ %while ((currprio = binprio(currop = (int) tok.ival)) > level)
	[ BIN_OP | PREF_OR_BIN_OP ] 
			{ *p = mknode(OP_BINOP, *p, (p_tree) 0);
			  (*p)->t_whichoper = currop;
			}
	expression(&((*p)->t_args[1]), currprio)
  |
	SEL_OP		{ *p = mknode(OP_BINOP, *p, (p_tree) 0);
			  (*p)->t_whichoper = (int) tok.ival;
			}
	name(&(*p)->t_args[1])
  |
	'['		{ *p = mknode(OP_BINOP, *p, (p_tree) 0);
			  (*p)->t_whichoper = E_ARRAY;
			}
	expression(&(*p)->t_args[1], 0)
	[	','	{ *p = mknode(OP_BINOP, *p, (p_tree) 0);
			  (*p)->t_whichoper = E_ARRAY;
			}
		expression(&(*p)->t_args[1], 0)
	]*
	']'
  ]*
			{ in_expression--; }
;

factor(p_tree *p;)
:
  [
  	%default EXPRESSION	/* lexical analyzer will never return this token */
			{ *p = mknode(OP_INTEGER, 0L); }
  |
  	'(' expression(p, 0) ')'
  |
  	INTEGER		{ *p = mknode(OP_INTEGER, tok.ival); }
  |
  	REAL		{ *p = mknode(OP_REAL, tok.fval); }
  |
  	STRING		{ *p = mknode(OP_STRING, tok.str); }
  |
  	qualified_name(p)
  |
  			{ *p = mknode(OP_UNOP, (p_tree) 0);
			  (*p)->t_whichoper = (int) tok.ival;
			}
  	[ PREF_OP 
  	| PREF_OR_BIN_OP
			{ (*currlang->fix_bin_to_pref)(*p); }
  	]
  	expression(&(*p)->t_args[0], unprio((*p)->t_whichoper))
  ]
  [ %while(1)
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
  [ STRING		{ str = tok.str; }
    ':'
  |			{ if (! listfile) str = 0;
			  else str = listfile->sy_idf->id_text;
			}
  ]
  count(&lin)		{ *p = mknode(OP_AT, lin->t_ival, str);
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

count(p_tree *p;)
:
  INTEGER		{ *p = mknode(OP_INTEGER, tok.ival); }
;

opt_num(p_tree *p;)
:
  num(p)
|
			{ *p = 0; }
;

num(p_tree *p;)
:
  count(p)
|
  '-' count(p)		{ (*p)->t_ival = - (*p)->t_ival; }
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
  | %default NAME
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
  | SET
  | TO
  | FIND
  | DISPLAY
  | WHICH
  | HELP
  | DISABLE
  | ENABLE
  | SOURCE
  | FRAME
  | LOG
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
	if (interrupted && c == EOF) {
		c = ' ';
		interrupted = 0;
		continue;
	}
  } while (c != EOF && class(c) == STSKIP);
  if (c == EOF) {
	eof_seen = 1;
	return c;
  }
  if (extended_charset && in_ext(c)) {
	TOK = get_name(c);
	return TOK;
  }
  switch(class(c)) {
  case STSTR:
	TOK = (*currlang->get_string)(c);
	break;
  case STIDF:
	if (in_expression) TOK = (*currlang->get_name)(c);
	else TOK = get_name(c);
	break;
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
  *p++ = 0;
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

extern char *symbol2str();

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

static void
catch_del()
{
  signal(SIGINT, catch_del);
  if (! disable_intr) {
  	signal_child(SIGEMT);
  	child_interrupted = 1;
  }
  interrupted = 1;
}

void
init_del()
{
  signal(SIGINT, catch_del);
}

static void
ctch()
{
  /* Only for shell escapes ... */
  signal(SIGINT, ctch);
}

#define SHBUFSIZ	512

static int
shellescape()
{
  register char *p;			/* walks through command */
  static char previous[SHBUFSIZ];	/* previous command */
  char comm[SHBUFSIZ];			/* space for command */
  register int cnt;			/* prevent array bound errors */
  register int c;			/* current char */
  register int lastc = 0;		/* will contain the previous char */

  p = comm;
  cnt = SHBUFSIZ-2;
  while (c = getc(db_in), c != '\n') {
	switch(c) {
	  case '!':
		/*
		 * An unescaped ! expands to the previous
		 * command, but disappears if there is none
		 */
		if (lastc != '\\') {
			if (*previous) {
				int len = strlen(previous);
				if ((cnt -= len) <= 0) break;
				strcpy(p,previous);
				p += len;
			}
		}
		else {
			*p++ = c;
		}
		continue;
	  case '%':
		/*
		 * An unescaped % will expand to the current
		 * filename, but disappears is there is none
		 */
		if (lastc != '\\') {
			if (listfile) {
				int len = strlen(listfile->sy_idf->id_text);
				if ((cnt -= len) <= 0) break;
				strcpy(p,listfile->sy_idf->id_text);
				p += len;
			}
		}
		else {
			*p++ = c;
		}
		continue;
	  default:
		lastc = c;
		if (cnt-- <= 0) break;
		*p++ = c;
		continue;
	}
	break;
  }
  *p = '\0';
  if (c != '\n') {
	warning("shell command too long");
  	while (c != '\n') c = getc(db_in);
  }
  ungetc(c, db_in);
  strcpy(previous, comm);
  signal(SIGINT, ctch);
  cnt = system(comm);
  signal(SIGINT, catch_del);
  return cnt;
}
}
