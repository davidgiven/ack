/* $Header$ */

#include	<stdio.h>
#include	<varargs.h>
#include	<assert.h>
#include	<alloc.h>
#include	<out.h>

#include	"operator.h"
#include	"position.h"
#include	"file.h"
#include	"idf.h"
#include	"tree.h"
#include	"message.h"
#include	"scope.h"
#include	"symbol.h"
#include	"langdep.h"
#include	"type.h"
#include	"expr.h"

extern FILE	*db_out;
extern t_lineno	currline;
static t_lineno	listline;
extern long	pointer_size;
extern char	*strrindex();
extern int	interrupted;

p_tree		print_command;

static int	wsize = 10;

/*VARARGS1*/
p_tree
mknode(va_alist)
  va_dcl
{
  va_list ap;
  register p_tree p = new_tree();

  va_start(ap);
  {
	register int i, na;

	p->t_oper = va_arg(ap, int);
	switch(p->t_oper) {
	case OP_NAME:
	case OP_HELP:
		p->t_idf = va_arg(ap, struct idf *);
		p->t_str = va_arg(ap, char *);
		break;
	case OP_STRING:
		p->t_sval = va_arg(ap, char *);
		break;
	case OP_REAL:
		p->t_fval = va_arg(ap, double);
		break;
	case OP_AT:
		p->t_lino = va_arg(ap, long);
		p->t_filename = va_arg(ap, char *);
		break;
	case OP_INTEGER:
	case OP_NEXT:
	case OP_STEP:
	case OP_REGS:
	case OP_RESTORE:
	case OP_WHERE:
		p->t_ival = va_arg(ap, long);
		break;
	default:
		na = nargs(p->t_oper);
		assert(na <= MAXARGS);
		for (i = 0; i < na; i++) {
			p->t_args[i] = va_arg(ap, p_tree);
		}
		break;
	}
  }
  va_end(ap);
  return p;
}

freenode(p)
  register p_tree	p;
{
  register int na, i;

  if (! p) return;
  na = nargs(p->t_oper);
  assert(na <= MAXARGS);
  for (i = 0; i < na; i++) {
	freenode(p->t_args[i]);
  }
  free_tree(p);
}

static t_addr
get_addr_from_node(p)
  p_tree	p;
{
  t_addr	a = ILL_ADDR;
  register p_symbol sym;

  if (! p) return NO_ADDR;
  if (p->t_address != 0) return p->t_address;
  switch(p->t_oper) {
  case OP_AT:
	if (! p->t_filename &&
	    (! listfile || ! (p->t_filename = listfile->sy_idf->id_text))) {
		error("no current file");
		break;
	}
	a = get_addr_from_position(&(p->t_pos));
	if (a == ILL_ADDR) {
		error("could not determine address of \"%s\":%d",
			p->t_filename, p->t_lino);
		break;
	}
	p->t_address = a;
	break;
	
  case OP_IN:
	a =  get_addr_from_node(p->t_args[0]);
	p->t_address = a;
	break;

  case OP_NAME:
  case OP_SELECT:
	sym = identify(p, FUNCTION|PROC|MODULE);
	if (! sym) {
		break;
	}
	if (! sym->sy_name.nm_scope || ! sym->sy_name.nm_scope->sc_bp_opp) {
		error("could not determine address of \"%s\"", p->t_str);
		break;
	}
	a = sym->sy_name.nm_scope->sc_bp_opp;
	break;

  default:
	assert(0);
  }
  return a;
}

print_node(p, top_level)
  register p_tree	p;
{
  if (!p) return;
  switch(p->t_oper) {
  case OP_LIST:
	fputs("list ", db_out);
	if (p->t_args[0]) {
		print_node(p->t_args[0], 0);
		if (p->t_args[1]) {
			if (p->t_args[1]->t_ival >= 0) {
				fputs(", ", db_out);
				print_node(p->t_args[1], 0);
			}
			else  {
				if (p->t_args[1]->t_ival < -100000000) {
					fputs("-", db_out);
				}
				else print_node(p->t_args[1], 0);
			}
		}
	}
	break;
  case OP_PRINT:
	fputs("print ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_ENABLE:
	fputs("enable ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_DISABLE:
	fputs("disable ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_DISPLAY:
	fputs("display ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_LINK:
	print_node(p->t_args[0], 0);
	fputs(", ", db_out);
	print_node(p->t_args[1], 0);
	break;
  case OP_FILE:
	fputs("file ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_SET:
	fputs("set ", db_out);
	print_node(p->t_args[0], 0);
	fputs(" to ", db_out);
	print_node(p->t_args[1], 0);
	break;
  case OP_FIND:
	fputs("find ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_WHICH:
	fputs("which ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_DELETE:
	fputs("delete ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_REGS:
	fprintf(db_out, "regs %ld", p->t_ival);
	break;
  case OP_NEXT:
	fprintf(db_out, "next %ld", p->t_ival);
	break;
  case OP_STEP:
	fprintf(db_out, "step %ld", p->t_ival);
	break;
  case OP_STATUS:
	fputs("status", db_out);
	break;
  case OP_DUMP:
	fputs("dump ", db_out);
	print_position(p->t_address, 1);
	break;
  case OP_RESTORE:
	fprintf(db_out, "restore %ld", p->t_ival);
	break;
  case OP_WHERE:
	fputs("where", db_out);
	if (p->t_ival != 0x7fffffff) fprintf(db_out, " %ld", p->t_ival);
	break;
  case OP_HELP:
	fputs("help", db_out);
	if (p->t_str != 0) fprintf(db_out, " %s", p->t_str);
	break;
  case OP_CONT:
	fputs("cont", db_out);
	if (p->t_args[0]) {
		fprintf(db_out, " %ld", p->t_args[0]->t_ival);
	}
	if (p->t_args[1]) {
		fputs(" ", db_out);
		print_node(p->t_args[1], 0);
	}
	break;

  case OP_WHEN:
	fputs("when ", db_out);
	if (p->t_address != NO_ADDR) {
		print_position(p->t_address, 1);
	}
	else print_node(p->t_args[0], 0);
	if (p->t_args[1]) {
		fputs(" if ", db_out);
		print_node(p->t_args[1], 0);
	}
	p = p->t_args[2];
	fputs(" { ", db_out);
	while (p->t_oper == OP_LINK) {
		print_node(p->t_args[0], 0);
		fputs("; ", db_out);
		p = p->t_args[1];
	}
	print_node(p, 0);
	fputs(" }", db_out);
	break;
  case OP_STOP:
	fputs("stop ", db_out);
	if (p->t_address != NO_ADDR) {
		print_position(p->t_address, 1);
	}
	else print_node(p->t_args[0], 0);
	if (p->t_args[1]) {
		fputs(" if ", db_out);
		print_node(p->t_args[1], 0);
	}
	break;
  case OP_TRACE:
	fputs("trace ", db_out);
	if (p->t_args[2]) {
		fputs("on ", db_out);
		print_node(p->t_args[2], 0);
		fputs(" ", db_out);
	}
	if (p->t_address != NO_ADDR) {
		print_position(p->t_address, 1);
	}
	else print_node(p->t_args[0], 0);
	if (p->t_args[1]) {
		fputs(" if ", db_out);
		print_node(p->t_args[1], 0);
	}
	break;
  case OP_AT:
	fprintf(db_out, "at \"%s\":%ld", p->t_filename, p->t_lino);
	break;
  case OP_IN:
	fputs("in ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_SELECT:
	print_node(p->t_args[0], 0);
	fputs("`", db_out);
	print_node(p->t_args[1], 0);
	break;
  case OP_NAME:
	fputs(p->t_str, db_out);
	break;
  case OP_INTEGER:
	fprintf(db_out, currlang->decint_fmt, p->t_ival);
	break;
  case OP_STRING:
	(*currlang->printstring)(p->t_sval, strlen(p->t_sval));
	break;
  case OP_REAL:
	fprintf(db_out, currlang->real_fmt, p->t_fval);
	break;
  case OP_FORMAT:
	print_node(p->t_args[0], 0);
	fputs("\\", db_out);
	print_node(p->t_args[1], 0);
	break;
  case OP_UNOP:
  case OP_BINOP:
	(*currlang->printop)(p);
	break;
  }
  if (top_level) fputs("\n", db_out);
}

int
repeatable(com)
  p_tree	com;
{
  switch(com->t_oper) {
  case OP_CONT:
	com->t_args[0]->t_ival = 1;
	freenode(com->t_args[1]);
	com->t_args[1] = 0;
	return 1;
  case OP_NEXT:
  case OP_STEP:
	com->t_ival = 1;
	return 1;
  case OP_LIST:
	freenode(com->t_args[0]);
	com->t_args[0] = 0;
	freenode(com->t_args[1]);
	com->t_args[1] = 0;
	return 1;
  }
  return 0;
}

int
in_status(com)
  p_tree	com;
{
  switch(com->t_oper) {
  case OP_STOP:
  case OP_WHEN:
  case OP_TRACE:
  case OP_DUMP:
  case OP_DISPLAY:
	return 1;
  }
  return 0;
}

eval(p)
  p_tree	p;
{
  if (p && operators[p->t_oper].op_fun) (*operators[p->t_oper].op_fun)(p);
}

do_list(p)
  p_tree	p;
{
  int	l1, l2;

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
	if (p->t_args[0]->t_oper == OP_INTEGER) {
		l1 = p->t_args[0]->t_ival;
	}
	else {
  		t_addr	a = get_addr_from_node(p->t_args[0]);
		p_position pos;

		if (a == ILL_ADDR) {
			return;
		}
		pos = get_position_from_addr(a);
  		newfile(str2idf(pos->filename, 1));
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

do_file(p)
  p_tree	p;
{
  if (p->t_args[0]) {
	newfile(p->t_args[0]->t_idf);
  }
  else if (listfile) fprintf(db_out, "%s\n", listfile->sy_idf->id_text);
  else error("no current file");
}

newfile(id)
  register struct idf	*id;
{
  register p_symbol sym = Lookup(id, PervasiveScope, FILESYM);

  if (listfile != sym) listline = 1;
  listfile = sym;
  if (! listfile) {
	listline = 1;
	listfile = add_file(id->id_text);
	listfile->sy_file->f_scope = FileScope;
  }
  find_language(strrindex(id->id_text, '.'));
}

setstop(p, kind)
  p_tree	p;
  int		kind;
{
  t_addr	a = get_addr_from_node(p->t_args[0]);

  if (a == ILL_ADDR) return 0;

  p->t_address = a;
  if (a != NO_ADDR) {
	if (! set_or_clear_breakpoint(a, kind)) {
		error("could not %s breakpoint", kind == SETBP ? "set" : "clear");
		return 0;
	}
  }
  return 1;
}

do_stop(p)
  p_tree	p;
{
  if (! setstop(p, SETBP)) {
	return;
  }
  add_to_item_list(p);
}

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
  if (! set_or_clear_trace(a, e, kind)) {
	error("could not %s trace", kind == SETTRACE ? "set" : "clear");
	return 0;
  }
  return 1;
}

do_trace(p)
  p_tree	p;
{
  p->t_address = NO_ADDR;
  if (! settrace(p, SETTRACE)) {
	return;
  }
  add_to_item_list(p);
}

static
able(p, kind)
  p_tree	p;
  int		kind;
{
  switch(p->t_oper) {
  case OP_LINK:
	able(p->t_args[0], kind);
	able(p->t_args[1], kind);
	break;
  case OP_INTEGER:
	able_item((int)p->t_ival, kind);
	break;
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

do_continue(p)
  p_tree	p;
{
  int count;
  int first_time = 1;

  if (p) {
	count = p->t_args[0]->t_ival;
	if (p->t_args[1]) {
		t_addr	a = get_addr_from_position(&(p->t_args[1]->t_pos));
		p_scope sc = get_scope_from_addr(a);

		if (a == ILL_ADDR || base_scope(sc) != base_scope(CurrentScope) ||
		    ! set_pc(a)) {
			error("cannot continue at line %d",
			      p->t_args[1]->t_lino);
			return;
		}
	}
  }
  else count = 1;
  while (count--) {
	if (! send_cont(count==0)) {
		if (first_time) error("no process");
		break;
	}
	first_time = 0;
  }
}

do_step(p)
  p_tree	p;
{
  if (! do_single_step(SETSS, p->t_ival)) {
	if (! interrupted) error("no process");
  }
}

do_next(p)
  p_tree	p;
{

  if (! do_single_step(SETSSF, p->t_ival)) {
	if (! interrupted) error("no process");
  }
}

extern t_addr	*get_EM_regs();

do_regs(p)
  p_tree	p;
{
  t_addr	*buf;
  int		n = p->t_ival;

  if (! (buf = get_EM_regs(n))) {
	if (! interrupted) error("no process");
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

/*ARGSUSED*/
do_where(p)
  p_tree	p;
{
  int i = 0;
  unsigned int cnt;
  unsigned int maxcnt = p->t_ival;
  p_scope sc;
  t_addr *buf;
  t_addr PC;

  if (p->t_ival < 0) {
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

extern p_tree	remove_from_item_list();

do_delete(p)
  p_tree	p;
{
  switch(p->t_oper) {
  case OP_DELETE:
	do_delete(p->t_args[0]);
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
		setstop(p, CLRBP);
		break;
  	case OP_TRACE:
		settrace(p, CLRTRACE);
		break;
  	case OP_DUMP:
		free_dump(p);
  	}
  	freenode(p);
	break;
  }
}

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
		freenode(print_command);
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
  if (! set_bytes(size, buf, a)) {
	error("could not handle this SET request");
  }
  free(buf);
}

perform(p, a)
  register p_tree	p;
  t_addr		a;
{
  switch(p->t_oper) {
  case OP_WHEN:
	if (p->t_args[1] && ! eval_cond(p->t_args[1])) break;
	p = p->t_args[2];
	while (p->t_oper == OP_LINK) {
		if (interrupted) return;
		eval(p->t_args[0]);
		p = p->t_args[1];
	}
	if (interrupted) return;
	eval(p);
	break;
  case OP_TRACE:
	if (p->t_args[0] && p->t_args[0]->t_oper == OP_IN) {
		register p_scope sc = base_scope(CurrentScope);
	
		if (sc != get_scope_from_addr(p->t_args[0]->t_address)) {
			break;
		}
	}
	if (interrupted) return;
	if (p->t_args[1] && ! eval_cond(p->t_args[1])) break;
	list_position(get_position_from_addr(a));
	if (p->t_args[2]) do_print(p->t_args[2]);
	break;
  default:
	assert(0);
  }
}

list_position(pos)
  p_position	pos;
{
  newfile(str2idf(pos->filename, 1));
  currfile = listfile;
  currline = pos->lineno;
  lines(currfile->sy_file, (int)currline, (int)1);
  listline = 0;
}
