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
#include	"scope.h"
#include	"symbol.h"
#include	"langdep.h"
#include	"type.h"
#include	"expr.h"
#include	"misc.h"

extern FILE	*db_out;
t_lineno	currline;
t_lineno	listline;
extern char	*strrindex();
extern int	interrupted;

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

t_addr
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

	if (p->t_args[1]) {
		p_scope sc;

		a = get_addr_from_node(p->t_args[1]);
		sc = base_scope(get_scope_from_addr(a));
		sym = identify(p->t_args[0], FUNCTION|PROC|MODULE);
		if (! sym->sy_name.nm_scope ||
		    ! sym->sy_name.nm_scope->sc_bp_opp) {
			error("could not determine address of \"%s\"", p->t_str);
			a = ILL_ADDR;
			break;
		}
		if (sc->sc_definedby != sym) {
			error("inconsistent address");
			a = ILL_ADDR;
			break;
		}
	}
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

static int	ommit_commas = 0;

print_node(f, p, top_level)
  register p_tree	p;
  register FILE		*f;
{
  if (!p) return;
  switch(p->t_oper) {
  case OP_LOG:
	fputs("log ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_PRCOMM:
	fputs("rerun ?", f);
	break;
  case OP_RUN:
	fputs("run ", f);
	ommit_commas = 1;
	print_node(f, p->t_args[0], 0);
	ommit_commas = 0;
	break;
  case OP_LIST:
	fputs("list ", f);
	if (p->t_args[0]) {
		print_node(f, p->t_args[0], 0);
		if (p->t_args[1]) {
			if (p->t_args[1]->t_ival >= 0) {
				fputs(", ", f);
				print_node(f, p->t_args[1], 0);
			}
			else  {
				if (p->t_args[1]->t_ival < -100000000) {
					fputs("-", f);
				}
				else print_node(f, p->t_args[1], 0);
			}
		}
	}
	break;
  case OP_PRINT:
	fputs("print ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_SOURCE:
	fputs("source ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_ENABLE:
	fputs("enable ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_DISABLE:
	fputs("disable ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_DISPLAY:
	fputs("display ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_LINK:
	print_node(f, p->t_args[0], 0);
	if (! ommit_commas) fputs(", ", f);
	else putc(' ', f);
	print_node(f, p->t_args[1], 0);
	break;
  case OP_FILE:
	fputs("file ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_FRAME:
	fputs("frame ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_UP:
	fputs("frame +", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_DOWN:
	fputs("frame -", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_SET:
	fputs("set ", f);
	print_node(f, p->t_args[0], 0);
	fputs(" to ", f);
	print_node(f, p->t_args[1], 0);
	break;
  case OP_FIND:
	fputs("find ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_WHICH:
	fputs("which ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_DELETE:
	fputs("delete ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_REGS:
	fputs("regs ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_NEXT:
	fputs("next ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_STEP:
	fputs("step ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_STATUS:
	fputs("status", f);
	break;
  case OP_DUMP:
	fputs("dump ", f);
	(void) print_position(p->t_address, 1);
	break;
  case OP_RESTORE:
	fputs("restore ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_WHERE:
	fputs("where ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_HELP:
	fputs("help ", f);
	print_node(f, p->t_args[0], 0);
	break;
  case OP_CONT:
	fputs("cont", f);
	if (p->t_args[0]) {
		fprintf(f, " %ld", p->t_args[0]->t_ival);
	}
	if (p->t_args[1]) {
		fputs(" ", f);
		print_node(f, p->t_args[1], 0);
	}
	break;

  case OP_WHEN:
	fputs("when ", f);
	if (p->t_address != NO_ADDR) {
		(void) print_position(p->t_address, 1);
	}
	else print_node(f, p->t_args[0], 0);
	if (p->t_args[1]) {
		fputs(" if ", f);
		print_node(f, p->t_args[1], 0);
	}
	p = p->t_args[2];
	fputs(" { ", f);
	while (p && p->t_oper == OP_LINK) {
		print_node(f, p->t_args[0], 0);
		fputs("; ", f);
		p = p->t_args[1];
	}
	print_node(f, p, 0);
	fputs(" }", f);
	break;
  case OP_STOP:
	fputs("stop ", f);
	if (p->t_address != NO_ADDR) {
		(void) print_position(p->t_address, 1);
	}
	else print_node(f, p->t_args[0], 0);
	if (p->t_args[1]) {
		fputs(" if ", f);
		print_node(f, p->t_args[1], 0);
	}
	break;
  case OP_TRACE:
	fputs("trace ", f);
	if (p->t_args[2]) {
		fputs("on ", f);
		print_node(f, p->t_args[2], 0);
		fputs(" ", f);
	}
	if (p->t_address != NO_ADDR) {
		(void) print_position(p->t_address, 1);
	}
	else print_node(f, p->t_args[0], 0);
	if (p->t_args[1]) {
		fputs(" if ", f);
		print_node(f, p->t_args[1], 0);
	}
	break;
  case OP_AT:
	fprintf(f, "at \"%s\":%d", p->t_filename, (int) p->t_lino);
	break;
  case OP_IN:
	fputs("in ", f);
	print_node(f, p->t_args[0], 0);
	fputs(" ", f);
	print_node(f, p->t_args[1], 0);
	break;
  case OP_SELECT:
	print_node(f, p->t_args[0], 0);
	fputs("`", f);
	print_node(f, p->t_args[1], 0);
	break;
  case OP_OUTPUT:
	fprintf(f, "> %s ", p->t_str);
	break;
  case OP_INPUT:
	fprintf(f, "< %s ", p->t_str);
	break;
  case OP_NAME:
	fputs(p->t_str, f);
	break;
  case OP_INTEGER:
	fprintf(f, currlang->decint_fmt, p->t_ival);
	break;
  case OP_STRING:
	(*currlang->printstring)(f, p->t_sval, strlen(p->t_sval));
	break;
  case OP_REAL:
	fprintf(f, currlang->real_fmt, p->t_fval);
	break;
  case OP_FORMAT:
	print_node(f, p->t_args[0], 0);
	fputs("\\", f);
	print_node(f, p->t_args[1], 0);
	break;
  case OP_UNOP:
  case OP_BINOP:
	(*currlang->printop)(f, p);
	break;
  default:
	assert(0);
  }
  if (top_level) fputs("\n", f);
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
	freenode(com->t_args[0]);
	com->t_args[0] = 0;
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
  case OP_PRCOMM:
	/* not really in status but may not be removed */
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

int	in_wheninvoked;

perform(p, a)
  register p_tree	p;
  t_addr		a;
{
  switch(p->t_oper) {
  case OP_WHEN:
	if (p->t_args[1] && ! eval_cond(p->t_args[1])) break;
	p = p->t_args[2];
	in_wheninvoked++;
	while (p && p->t_oper == OP_LINK) {
		if (interrupted) return;
		if (p->t_args[0]) eval(p->t_args[0]);
		p = p->t_args[1];
	}
	if (interrupted) return;
	if (p) eval(p);
	in_wheninvoked--;
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
