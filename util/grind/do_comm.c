/* $Header$ */

/* Implementation of the do_ routines */

#include <stdio.h>
#include <assert.h>
#include <alloc.h>

#include "operator.h"
#include "position.h"
#include "tree.h"
#include "idf.h"
#include "Lpars.h"
#include "type.h"
#include "expr.h"
#include "symbol.h"
#include "scope.h"
#include "file.h"
#include "message.h"

extern FILE	*db_out;
extern t_lineno	listline, currline;
extern int	stop_reason;
extern int	interrupted;

p_tree		print_command;

/* ------------------------------------------------------------- */

/* implementation of the help command */

do_help(p)
  p_tree	p;
{
  p = p->t_args[0];
  if (p && p->t_idf) switch(p->t_idf->id_reserved) {
  case HELP:
	fputs("help [ <commandname> ]\n", db_out);
	fputs("? [ <commandname> ]\n", db_out);
	fputs("  Print a command summary, or some more help on <commandname>.\n", db_out);
	return;
  case LIST:
	fputs("list [ <start> | <func> ] [ , <cnt> | - [ <end> ] ]\n", db_out);
	fputs("l [ <start> | <func> ] [ , <cnt> | - [ <end> ] ]\n", db_out);
	fputs("  List lines from the current source file, starting with either\n", db_out);
	fputs("  line <start> or some lines before the first statement of <func> or\n", db_out);
	fputs("  the current line. Either list <cnt> lines or <wsize> lines,\n", db_out);
	fputs("  except when a range is given.\n", db_out);
	fputs("  <wsize> is the last <cnt> given, or 10.\n", db_out);
	return;
  case XFILE:
	fputs("file [ <name> ]\n", db_out);
	fputs("  Print the name of the current source file, or change the\n", db_out);
	fputs("  current source file to <name>.\n", db_out);
	return;
  case RUN:
	fputs("run [ <args> ] [ < <infile> ] [ > <outfile> ]\n", db_out);
	fputs("  Start executing debuggee with command line arguments <args> and\n", db_out);
	fputs("  possible redirection of standard input and/or standard output.\n", db_out);
	return;
  case RERUN:
	fputs("rerun\n", db_out);
	fputs("  repeats the last run command.\n", db_out);
	return;
  case STOP:
	fputs("stop [ <pos> ] [ if <cond> ]\n", db_out);
	fputs("  Stop execution when position <pos> is reached, and then when\n", db_out);
	fputs("  <cond> becomes true. If no <pos> is given, stop when <cond>\n", db_out);
	fputs("  becomes true.  If no <cond> is given, stop when <pos> is reached.\n", db_out);
	fputs("  Either a position or a condition (or both) must be given.\n", db_out);
	return;
  case WHEN:
	fputs("when [ <pos> ] [ if <cond> ] { <command> [ ; <command> ] ... } \n", db_out);
	fputs("  Execute the <command>s when position <pos> is reached, and then when\n", db_out);
	fputs("  <cond> becomes true. If no <pos> is given, do this when <cond>\n", db_out);
	fputs("  becomes true.  If no <cond> is given, do this when <pos> is reached.\n", db_out);
	fputs("  Either a position or a condition (or both) must be given.\n", db_out);
	return;
  case CONT:
	fputs("cont [ <cnt> ] [ at <line> ]\n", db_out);
	fputs("c [ <cnt> ] [ at <line> ]\n", db_out);
	fputs("  Continue execution, skipping <cnt> or 1 breakpoints;a\n", db_out);
	fputs("  if <line> is given, continue at <line>.\n", db_out);
	return;
  case STEP:
  case NEXT:
	fputs("step [ <cnt> ]\n", db_out);
	fputs("s [ <cnt> ]\n", db_out);
	fputs("next [ <cnt> ]\n", db_out);
	fputs("n [ <cnt> ]\n", db_out);
	fputs("  Execute the next <cnt> or 1 source line(s).\n", db_out);
	fputs("  Step (s) steps into function-calls.\n", db_out);
	fputs("  Next (n) steps past function-calls.\n", db_out);
	return;
  case WHERE:
	fputs("where [ <cnt> ]\n", db_out);
	fputs("w [ <cnt> ]\n", db_out);
	fputs("  List all, or the top <cnt> or the bottom -<cnt> active functions.\n", db_out);
	return;
  case STATUS:
	fputs("status\n", db_out);
	fputs("  display active traces, stops, whens, displays, and dumps.\n", db_out);
	return;
  case DELETE:
	fputs("delete [ <num> [ , <num> ] ... ]\n", db_out);
	fputs("d [ <num> [ , <num> ] ...] \n", db_out);
	fputs("  Remove the command(s) corresponding to <num> (as displayed by 'status').\n", db_out);
	fputs("  If no <num> is given, remove the current stopping point.\n", db_out);
	return;
  case SET:
	fputs("set <desig> to <exp>\n", db_out);
	fputs("  Assign the value of <exp> to <desig>.\n", db_out);
	return;
  case PRINT:
	fputs("print <exp> [ , <exp> ] ...\n", db_out);
	fputs("p <exp> [ , <exp> ] ...\n", db_out);
	fputs("  Print the value of each <exp>.\n", db_out);
	return;
  case DISPLAY:
	fputs("display <exp> [ , <exp> ] ...\n", db_out);
	fputs("  Print the value of each <exp> whenever the debuggee stops.\n", db_out);
	return;
  case DUMP:
	fputs("dump\n", db_out);
	fputs("  Saves the state of the debuggee; it can be restored with the restore command.\n", db_out);
	return;
  case RESTORE:
	fputs("restore [ <num> ]\n", db_out);
	fputs("r [ <num> ]\n", db_out);
	fputs("  Restore the state of the dump associated with <num>,\n", db_out);
	fputs("  or restore the state of the last dump.\n", db_out);
	return;
  case TRACE:
	fputs("trace [ on <exp> ] [ <pos> ] [ if <cond> ]\n", db_out);
	fputs("t [ on <exp> ] [ <pos> ] [ if <cond> ]\n", db_out);
	fputs("  Without args, display each source line before execution.\n", db_out);
	fputs("  In addition, display <exp> in the on-clause.\n", db_out);
	fputs("  If <pos> is given and indicates a function, only display\n", db_out);
	fputs("  tracing information while executing this function.\n", db_out);
	fputs("  If it indicates a line number, only display tracing information\n", db_out);
	fputs("  whenever the source line is reached.\n", db_out);
	fputs("  If <cond> is given, only display tracing info when it evaluates to non-zero.\n", db_out);
	return;
  case FIND:
	fputs("find <name>\n", db_out);
	fputs("  Prints the fully qualified name of all symbols matching <name>.\n", db_out);
	return;
  case WHICH:
	fputs("which <name>\n", db_out);
	fputs("  Prints the fully qualified name of <name>.\n", db_out);
	return;
  case DISABLE:
	fputs("disable [ <num> [ , <num> ] ... ]\n", db_out);
	fputs("  Disable the command(s) corresponding to <num> (as displayed by 'status').\n", db_out);
	fputs("  If no <num> is given, disable the current stopping point.\n", db_out);
	return;
  case ENABLE:
	fputs("enable [ <num> [ , <num> ] ... ]\n", db_out);
	fputs("  Enable the command(s) corresponding to <num> (as displayed by 'status'.)\n", db_out);
	fputs("  If no <num> is given, enable the current stopping point (not effective).\n", db_out);
	return;
  }
  fputs("cont [ <cnt> ] [ at <line> ]\n", db_out);
  fputs("delete [ <num> [ , <num> ] ... ]\n", db_out);
  fputs("disable [ <num> [ , <num> ] ... ]\n", db_out);
  fputs("display <exp> [ , <exp> ] ...\n", db_out);
  fputs("dump\n", db_out);
  fputs("enable [ <num> [ , <num> ] ... ]\n", db_out);
  fputs("file [ <name> ]\n", db_out);
  fputs("find <name>\n", db_out);
  fputs("help [ <commandname> ]\n", db_out);
  fputs("list [ <start> | <func> ] [ , <cnt> | - [ <end> ] ]\n", db_out);
  fputs("next [ <cnt> ]\n", db_out);
  fputs("print <exp> [ , <exp> ] ...\n", db_out);
  fputs("rerun\n", db_out);
  fputs("restore <num>\n", db_out);
  fputs("run [ <args> ] [ < <infile> ] [ > <outfile> ]\n", db_out);
  fputs("set <desig> to <exp>\n", db_out);
  fputs("status\n", db_out);
  fputs("step [ <cnt> ]\n", db_out);
  fputs("stop [ <pos> ] [ if <cond> ]\n", db_out);
  fputs("trace [ on <exp> ] [ <pos> ] [ if <cond> ]\n", db_out);
  fputs("when [ <pos> ] [ if <cond> ] { <command> [ ; <command> ] ... } \n", db_out);
  fputs("where [ <cnt> ]\n", db_out);
  fputs("which <name>\n", db_out);
}

/* ------------------------------------------------------------- */

/* implementation of dump/restore commands */

extern long	pointer_size;
extern p_tree	get_from_item_list();

struct dump {
  char	*globals, *stack;
  struct message_hdr mglobal, mstack;
  struct dump *next;
};

static struct dump	*last_dump;

do_dump(p)
  p_tree	p;
{
  struct dump *d = (struct dump *) malloc(sizeof(struct dump));

  if (! d) {
	error("could not allocate enough memory");
	return;
  }
  if (! get_dump(&d->mglobal, &d->globals, &d->mstack, &d->stack)) {
	free((char *) d);
	return;
  }
  p->t_args[0] = (struct tree *) d;
  p->t_address = (t_addr) get_int(d->mglobal.m_buf+PC_OFF*pointer_size, pointer_size, T_UNSIGNED);
  add_to_item_list(p);
  d->next = last_dump;
  last_dump = d;
}

do_restore(p)
  p_tree	p;
{
  struct dump *d;
  
  if (p->t_args[0]) { 
	p = get_from_item_list((int) p->t_args[0]->t_ival);
  	if (!p || p->t_oper != OP_DUMP) {
		error("no such dump");
		return;
	}
  	d = (struct dump *) p->t_args[0];
  }
  else	d = last_dump;

  if (! d) {
	error("no dumps");
	return;
  }

  if (! put_dump(&d->mglobal, d->globals, &d->mstack, d->stack)) {
  }
  perform_items();
}

free_dump(p)
  p_tree	p;
{
  struct dump *d = (struct dump *) p->t_args[0];

  free(d->globals);
  free(d->stack);
  if (d == last_dump) last_dump = d->next;
  else {
	register struct dump *d1 = last_dump;

	while (d1->next != d) d1 = d1->next;
	d1->next = d->next;
  }
  free((char *) d);
}

/* ------------------------------------------------------------- */

/* implementation of the find command */

do_find(p)
  p_tree	p;
{
  /* Print all identifications of p->t_args[0]. */
  register p_symbol s;
  p_tree	arg;

  p = p->t_args[0];
  switch(p->t_oper) {
  case OP_NAME:
	s = p->t_idf->id_def;
	while (s) {
		pr_sym(s);
		s = s->sy_next;
	}
	break;

  case OP_SELECT:
	arg = p->t_args[1];
	assert(arg->t_oper == OP_NAME);
	s = arg->t_idf->id_def;
	while (s) {
		if (consistent(p, s->sy_scope)) {
			pr_sym(s);
		}
		s = s->sy_next;
	}
	break;

  default:
	assert(0);
  }
}

/* ------------------------------------------------------------- */

/* implementation of the which command */

do_which(p)
  p_tree	p;
{
  p_symbol	sym = identify(p->t_args[0], 0xffff);

  if ( sym) pr_sym(sym);
}

/* ------------------------------------------------------------- */

/* implementation of the list command */

extern t_addr	get_addr_from_node();

do_list(p)
  p_tree	p;
{
  int	l1, l2;
  static int wsize = 10;

  if (p->t_args[1]) {
	l2 = p->t_args[1]->t_ival;
	if (l2 >= 0) {
		if (l2 == 0) l2 = 1;
		wsize = l2;
	}
  }
  else l2 = wsize;

  if (! p->t_args[0]) {
	l1 = listline;
	if (! l1) {
		listline = currline - (wsize/2);
		l1 = listline;
	}
  }
  else {
	if (p->t_args[0]->t_oper == OP_AT) {
		l1 = p->t_args[0]->t_lino;
		if (p->t_args[0]->t_filename) {
			newfile(str2idf(p->t_args[0]->t_filename, 0));
		}
	}
	else {
  		t_addr	a = get_addr_from_node(p->t_args[0]);
		p_position pos;
		p_symbol oldlistfile = listfile;

		if (a == ILL_ADDR) {
			return;
		}
		pos = get_position_from_addr(a);
  		newfile(str2idf(pos->filename, 1));
		if (listfile != oldlistfile) {
			warning("switching to file %s", listfile->sy_idf->id_text);
		}
		l1 = pos->lineno - (l2 > 0 ? l2 : wsize)/2;
		if (l1 < 1) l1 = 1;
	}
  }
  if (listfile) {
	if (l2 < 0) {
		l2 = -l2;
		if (l1 > l2) l2 = 1;
		else l2 -= l1 - 1;
	}
	lines(listfile->sy_file, l1, l2);
	listline = l1 + l2;
  }
  else error("no current file");
}

/* ------------------------------------------------------------- */

/* implementation of the file command */

do_file(p)
  p_tree	p;
{
  FILE	*f;

  if (p->t_args[0]) {
	if (! strcmp(p->t_args[0]->t_str, "?")) {
		register p_symbol	sym = PervasiveScope->sc_symbs;

		while (sym) {
			if (sym->sy_class == FILESYM) {
				fprintf(db_out, "%s\n", sym->sy_idf->id_text);
			}
			sym = sym->sy_prev_sc;
		}
		return;
	}
	if ((f = fopen(p->t_args[0]->t_str, "r")) == NULL) {
		error("could not open %s", p->t_args[0]->t_str);
		return;
	}
	fclose(f);
	newfile(p->t_args[0]->t_idf);
  }
  else if (listfile) fprintf(db_out, "%s\n", listfile->sy_idf->id_text);
  else error("no current file");
}

/* ------------------------------------------------------------- */

/* implementation of stop/when command */

setstop(p, kind)
  p_tree	p;
  int		kind;
{
  t_addr	a = get_addr_from_node(p->t_args[0]);

  if (a == ILL_ADDR) return 0;

  p->t_address = a;
  if (a != NO_ADDR) {
	if (! set_or_clear_breakpoint(a, kind)) {
		return 0;
	}
  }
  return 1;
}

do_stop(p)
  p_tree	p;
{
  if (! setstop(p, M_SETBP)) {
	return;
  }
  add_to_item_list(p);
}

/* ------------------------------------------------------------- */

/* implementation of the trace command */

settrace(p, kind)
  p_tree	p;
  int		kind;
{
  t_addr	a, e;

  a = get_addr_from_node(p->t_args[0]);
  if (a == NO_ADDR) return 1;
  if (a == ILL_ADDR) return 0;
  if (p->t_args[0]->t_oper == OP_AT) {
	e = a;
	p->t_address = a;
  }
  else {
	p_scope sc = get_next_scope_from_addr(a+1);

	if (sc) e = sc->sc_start - 1;
	else e = 0xffffffff;
  }
  return set_or_clear_trace(a, e, kind);
}

do_trace(p)
  p_tree	p;
{
  p->t_address = NO_ADDR;
  if (! settrace(p, M_SETTRACE)) {
	return;
  }
  add_to_item_list(p);
}

/* ------------------------------------------------------------- */

/* implementation of the enable/disable commands */

static
able(p, kind)
  p_tree	p;
  int		kind;
{
  if (!p) {
	if (stop_reason) {
		able_item(stop_reason, kind);
	}
	else {
		error("no current stopping point");
	}
	return;
  }
  switch(p->t_oper) {
  case OP_LINK:
	able(p->t_args[0], kind);
	able(p->t_args[1], kind);
	break;
  case OP_INTEGER:
	able_item((int)p->t_ival, kind);
	break;
  default:
	assert(0);
  }
}

do_enable(p)
  p_tree	p;
{
  able(p->t_args[0], 0);
}

do_disable(p)
  p_tree	p;
{
  able(p->t_args[0], 1);
}

/* ------------------------------------------------------------- */

/* implementation of the cont command */

do_continue(p)
  p_tree	p;
{
  int count;

  if (p) {
	count = p->t_args[0]->t_ival;
	if (p->t_args[1]) {
		t_addr	a = get_addr_from_position(&(p->t_args[1]->t_pos));
		p_scope sc = get_scope_from_addr(a);

		if (a == ILL_ADDR || base_scope(sc) != base_scope(CurrentScope)) {
			error("cannot continue at line %d",
			      p->t_args[1]->t_lino);
			return;
		}
		if (! set_pc(a)) {
			return;
		}
	}
  }
  else count = 1;
  while (count--) {
	if (! send_cont(count==0)) {
		break;
	}
  }
  if (count > 0) {
	fprintf(db_out, "Only %d breakpoints skipped\n",
		p->t_args[0]->t_ival - count);
  }
}

/* ------------------------------------------------------------- */

/* implementation of the step command */

do_step(p)
  p_tree	p;
{
  p = p->t_args[0];
  if (! singlestep(M_SETSS, p ? p->t_ival : 1L)) {
  }
}

/* ------------------------------------------------------------- */

/* implementation of the next command */

do_next(p)
  p_tree	p;
{
  p = p->t_args[0];
  if (! singlestep(M_SETSSF, p? p->t_ival : 1L)) {
  }
}

extern t_addr	*get_EM_regs();

/* ------------------------------------------------------------- */

/* implementation of the regs command (temporarily) */

do_regs(p)
  p_tree	p;
{
  t_addr	*buf;
  int		n = 0;

  p = p->t_args[0];
  if (p) n = p->t_ival;
  if (! (buf = get_EM_regs(n))) {
	return;
  }
  fprintf(db_out, "EM registers %d levels back:\n", n);
  fprintf(db_out, "\tLocalBase =\t0x%lx\n\tArgumentBase =\t0x%lx\n", 
		(long) buf[LB_OFF], (long) buf[AB_OFF]);
  fprintf(db_out, "\tProgramCounter=\t0x%lx\n\tHeapPointer = \t0x%lx\n",
		(long) buf[PC_OFF],
		(long) buf[HP_OFF]);
  fprintf(db_out, "\tStackPointer =\t0x%lx\n", (long) buf[SP_OFF]);
}

/* ------------------------------------------------------------- */

/* implementation of the where command */

/*ARGSUSED*/
do_where(p)
  p_tree	p;
{
  int i = 0;
  unsigned int cnt;
  unsigned int maxcnt = 0xffff;
  p_scope sc;
  t_addr *buf;
  t_addr PC;

  p = p->t_args[0];
  if (p && p->t_ival < 0) {
	for (;;) {
		buf = get_EM_regs(i++);
		if (! buf || ! buf[AB_OFF]) break;
		PC = buf[PC_OFF];
		sc = base_scope(get_scope_from_addr(PC));
		if (! sc || sc->sc_start > PC) break;
		if (interrupted) return;
	}
	i--;
	maxcnt = - p->t_ival;
	i -= maxcnt;
	if (i < 0) i = 0;
  }
  else if (p) maxcnt = p->t_ival;
  for (cnt = maxcnt; cnt != 0; cnt--) {
	t_addr AB;

	if (interrupted) return;
	if (! (buf = get_EM_regs(i++))) break;
	AB = buf[AB_OFF];
	PC = buf[PC_OFF];
	if (! AB) break;
	sc = base_scope(get_scope_from_addr(PC));
	if (! sc || sc->sc_start > PC) break;
	fprintf(db_out, "%s(", sc->sc_definedby->sy_idf->id_text);
	print_params(sc->sc_definedby->sy_type, AB, has_static_link(sc));
	fputs(") ", db_out);
	print_position(PC, 0);
	fputs("\n", db_out);
  }
}

/* ------------------------------------------------------------- */

/* implementation of the delete command */

extern p_tree	remove_from_item_list();

do_delete(p)
  p_tree	p;
{
  switch(p->t_oper) {
  case OP_DELETE:
	if (! p->t_args[0]) {
		if (stop_reason) {
			remove_from_item_list(stop_reason);
			stop_reason = 0;
		}
		else {
			error("no current stopping point");
		}
	}
	else do_delete(p->t_args[0]);
	break;
  case OP_LINK:
	do_delete(p->t_args[0]);
	do_delete(p->t_args[1]);
	break;
  case OP_INTEGER:
  	p = remove_from_item_list((int) p->t_ival);

  	if (p) switch(p->t_oper) {
  	case OP_WHEN:
  	case OP_STOP:
		setstop(p, M_CLRBP);
		break;
  	case OP_TRACE:
		settrace(p, M_CLRTRACE);
		break;
  	case OP_DUMP:
		free_dump(p);
  	}
  	freenode(p);
	break;
  default:
	assert(0);
  }
}

/* ------------------------------------------------------------- */

/* implementation of the print command */

do_print(p)
  p_tree	p;
{
  char	*buf = 0;
  char *format = 0;
  long	size;
  p_type tp;

  switch(p->t_oper) {
  case OP_PRINT:
	if (p->t_args[0] == 0) {
		p = print_command;
		if (p == 0) {
			error("no previous print command");
			break;
		}
	}
	else if (p != print_command) {
		/* freenode(print_command); No, could be in when-list */
		print_command = p;
	}
	/* fall through */
  case OP_DISPLAY:
	do_print(p->t_args[0]);
	break;
  case OP_LINK:
	do_print(p->t_args[0]);
	do_print(p->t_args[1]);
	break;
  default:
	if (interrupted || ! eval_expr(p, &buf, &size, &tp)) return;
	print_node(p, 0);
	fputs(" = ", db_out);
	if (p->t_oper == OP_FORMAT) {
		format = p->t_args[1]->t_str;
	}
	print_val(tp, size, buf, 0, 0, format);
	if (buf) free(buf);
	fputs("\n", db_out);
	break;
  }
}

/* ------------------------------------------------------------- */

/* implementation of the set command */

do_set(p)
  p_tree	p;
{
  char	*buf = 0;
  long	size, size2;
  p_type tp, tp2;
  t_addr a;

  if (interrupted || ! eval_desig(p->t_args[0], &a, &size, &tp) ||
      ! eval_expr(p->t_args[1], &buf, &size2, &tp2) ||
      ! convert(&buf, &size2, &tp2, tp, size)) {
	if (buf) free(buf);
	return;
  }

  if (interrupted) {
	free(buf);
	return;
  }
  set_bytes(size, buf, a);
  free(buf);
}

