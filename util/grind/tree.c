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

extern FILE	*db_out;
extern t_lineno	currline;
extern long	pointer_size;
extern char	*strrindex();

p_tree		run_command;

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
		p->t_idf = va_arg(ap, struct idf *);
		p->t_str = va_arg(ap, char *);
		break;
	case OP_INTEGER:
		p->t_ival = va_arg(ap, long);
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
	case OP_NEXT:
	case OP_STEP:
	case OP_REGS:
	case OP_DELETE:
	case OP_RESTORE:
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
			fputs(", ", db_out);
			print_node(p->t_args[1], 0);
		}
	}
	break;
  case OP_PRINT:
	fputs("print ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_FILE:
	fputs("file ", db_out);
	print_node(p->t_args[0], 0);
	break;
  case OP_DELETE:
	fprintf(db_out, "delete %d", p->t_ival);
	break;
  case OP_REGS:
	fprintf(db_out, "regs %d", p->t_ival);
	break;
  case OP_NEXT:
	fprintf(db_out, "next %d", p->t_ival);
	break;
  case OP_STEP:
	fprintf(db_out, "step %d", p->t_ival);
	break;
  case OP_STATUS:
	fputs("status", db_out);
	break;
  case OP_DUMP:
	fputs("dump ", db_out);
	print_position(p->t_address, 1);
	break;
  case OP_RESTORE:
	fprintf(db_out, "restore %d", p->t_ival);
	break;
  case OP_WHERE:
	fputs("where", db_out);
	break;
  case OP_CONT:
	fputs("cont", db_out);
	if (p->t_args[0]) {
		fprintf(db_out, " %d", p->t_args[0]->t_ival);
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
	fprintf(db_out, "%d", p->t_ival);
	break;
  case OP_STRING:
	fprintf(db_out, "%s", p->t_sval);
	break;
  case OP_REAL:
	fprintf(db_out, "%.14g", p->t_fval);
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
  case OP_NEXT:
  case OP_STEP:
  case OP_LIST:
  case OP_STATUS:
  case OP_PRINT:
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
	return 1;
  }
  return 0;
}

eval(p)
  p_tree	p;
{
  if (p) (*operators[p->t_oper].op_fun)(p);
}

do_list(p)
  p_tree	p;
{
  if (currfile) {
	lines(currfile->sy_file,
	      p->t_args[0] ? (int) p->t_args[0]->t_ival : (int) currline,
	      p->t_args[1] ? (int) p->t_args[1]->t_ival : (int) currline+9);
	currline = p->t_args[1] ? p->t_args[1]->t_ival + 1 : currline + 10;
  }
  else fprintf(db_out, "no current file\n");
}

do_file(p)
  p_tree	p;
{
  if (p->t_args[0]) {
	newfile(p->t_args[0]->t_idf);
  }
  else if (currfile) fprintf(db_out, "%s\n", currfile->sy_idf->id_text);
  else fprintf(db_out, "no current file\n");
}

newfile(id)
  register struct idf	*id;
{
  register p_symbol sym = Lookup(id, PervasiveScope, FILESYM);

  if (currfile != sym) currline = 1;
  currfile = sym;
  if (! currfile) {
	currline = 1;
	currfile = add_file(id->id_text);
	currfile->sy_file->f_scope = FileScope;
  }
  find_language(strrindex(id->id_text, '.'));
}

static t_addr
get_pos(p)
  p_tree	p;
{
  t_addr	a = ILL_ADDR;
  register p_symbol sym;

  if (! p) return NO_ADDR;
  if (p->t_address != 0) return p->t_address;
  switch(p->t_oper) {
  case OP_AT:
	if (! p->t_filename &&
	    (! currfile || ! (p->t_filename = currfile->sy_idf->id_text))) {
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
	a =  get_pos(p->t_args[0]);
	p->t_address = a;
	break;

  case OP_NAME:
  case OP_SELECT:
	sym = identify(p, PROC|MODULE);
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

do_stop(p)
  p_tree	p;
{
  t_addr	a = get_pos(p->t_args[0]);

  if (a == ILL_ADDR) {
	return;
  }

  p->t_address = a;
  add_to_item_list(p);
  if (a != NO_ADDR) {
	if (! set_or_clear_breakpoint(a, SETBP)) {
		error("could not set breakpoint");
	}
  }
}

do_trace(p)
  p_tree	p;
{
  t_addr a;
  t_addr e;

  p->t_address = NO_ADDR;
  if (p->t_args[0]) {
	a = get_pos(p->t_args[0]);
	if (a == ILL_ADDR) return;
	if (p->t_args[0]->t_oper == OP_AT) {
		e = a;
		p->t_address = a;
	}
	else {
		p_scope sc = get_next_scope_from_addr(a+1);

		if (sc) e = sc->sc_start - 1;
		else e = 0xffffffff;
	}
	if (! set_or_clear_trace(a, e, SETTRACE)) {
		error("could not set trace");
	}
  }
  add_to_item_list(p);
}

do_continue(p)
  p_tree	p;
{
  int count;

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
		error("no debuggee");
		break;
	}
  }
}

do_step(p)
  p_tree	p;
{
  if (! do_single_step(SETSS, p->t_ival)) {
	error("no debuggee");
  }
}

do_next(p)
  p_tree	p;
{

  if (! do_single_step(SETSSF, p->t_ival)) {
	error("no debuggee");
  }
}

extern t_addr	*get_EM_regs();

do_regs(p)
  p_tree	p;
{
  t_addr	*buf;
  int		n = p->t_ival;

  if (! (buf = get_EM_regs(n))) {
	error("no debuggee");
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

  for (;;) {
	t_addr AB;
	t_addr PC;
	p_scope sc;
	t_addr *buf;

	if (! (buf = get_EM_regs(i++))) {
		error("no debuggee");
		return;
	}
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

/*ARGSUSED*/
do_status(p)
  p_tree	p;
{
  print_items();
}

extern p_tree	remove_from_item_list();

do_delete(p)
  p_tree	p;
{
  p = remove_from_item_list((int) p->t_ival);

  if (p) switch(p->t_oper) {
  case OP_WHEN:
  case OP_STOP: {
	t_addr a = get_pos(p->t_args[0]);

	if (a != ILL_ADDR && a != NO_ADDR) {
		set_or_clear_breakpoint(a, CLRBP);
	}
	break;
	}
  case OP_TRACE: {
	t_addr a = get_pos(p->t_args[0]);
	
	if (a != ILL_ADDR && a != NO_ADDR) {
		t_addr e;
		if (p->t_args[0]->t_oper == OP_AT) {
			e = a;
		}
		else {
			p_scope sc = get_next_scope_from_addr(a+1);

			if (sc) e = sc->sc_start - 1;
			else e = 0xffffffff;
		}
		set_or_clear_trace(a, e, CLRTRACE);
	}
	break;
	}
  case OP_DUMP:
	free_dump(p);
  }
  freenode(p);
}

do_print(p)
  p_tree	p;
{
  p_symbol sym;

  switch(p->t_oper) {
  case OP_PRINT:
	do_print(p->t_args[0]);
	break;
  case OP_LINK:
	do_print(p->t_args[0]);
	do_print(p->t_args[1]);
	break;
  case OP_NAME:
  case OP_SELECT:
	sym = identify(p, VAR|REGVAR|LOCVAR|VARPAR|CONST);
	if (! sym) return;
	print_node(p, 0);
	if (! print_sym(sym)) {
		fputs(" currently not available\n", db_out);
		break;
	}
  }
}

perform(p, a)
  register p_tree	p;
  t_addr		a;
{
  switch(p->t_oper) {
  case OP_WHEN:
	p = p->t_args[2];
	while (p->t_oper == OP_LINK) {
		eval(p->t_args[0]);
		p = p->t_args[1];
	}
	eval(p);
	break;
  case OP_TRACE:
	if (p->t_args[0] && p->t_args[0]->t_oper == OP_IN) {
		register p_scope sc = base_scope(CurrentScope);
	
		if (sc != get_scope_from_addr(p->t_args[0]->t_address)) {
			break;
		}
	}
	{
		p_position pos = get_position_from_addr(a);

		newfile(str2idf(pos->filename, 1));
		currline = pos->lineno;
		lines(currfile->sy_file, (int)currline, (int)currline);
		if (p->t_args[2]) do_print(p->t_args[2]);
	}
	break;
  default:
	assert(0);
  }
}
