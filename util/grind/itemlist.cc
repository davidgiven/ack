/* $Header$ */

#include <alloc.h>
#include <stdio.h>
#include <assert.h>

#include "position.h"
#include "tree.h"
#include "operator.h"
#include "message.h"

extern FILE	*db_out;
extern int	db_ss;
int		stop_reason;

typedef struct item {
  struct item	*i_next;
  struct tree	*i_node;
  int		i_disabled;
  int		i_itemno;
} t_item, *p_item;

/* STATICALLOCDEF "item" 10 */

struct itemlist {
  p_item	il_first, il_last;
  int		il_count;
};

static struct itemlist	item_list;

static int
in_item_list(p)
  p_tree	p;
{
  register p_item i = item_list.il_first;

  while (i) {
	if (i->i_node == p) return 1;
	i = i->i_next;
  }
  return 0;
}

static
pr_item(i)
  p_item	i;
{
  fprintf(db_out, "(%d)\t", i->i_itemno);
  print_node(i->i_node, 0);
  fputs(i->i_disabled ? " (disabled)\n": "\n", db_out);
}

int
item_addr_actions(a, mess_type, may_stop)
  t_addr	a;
  int		mess_type;
  int		may_stop;
{
  /* Perform actions associated with position 'a', and return 1 if we must stop
     there, and 0 if not.
  */
  register p_item i = item_list.il_first;
  int stopping = 0;

  stop_reason = 0;
  for (i = item_list.il_first; i != 0; i = i->i_next) {
	register p_tree	p = i->i_node;

	if (! i->i_disabled
	    && (p->t_address == a || p->t_address == NO_ADDR)) {
		switch(p->t_oper) {
		case OP_STOP:
			if (mess_type != DB_SS && mess_type != OK) break;
			if (! p->t_args[1] || eval_cond(p->t_args[1])) {
				if (! stop_reason) stop_reason = i->i_itemno;
				stopping = 1;
			}
			break;
		case OP_TRACE:
		case OP_WHEN:
		case OP_DUMP:
		case OP_DISPLAY:
			break;
		default:
			assert(0);
		}
	}
  }
  for (i = item_list.il_first; i != 0; i = i->i_next) {
	register p_tree	p = i->i_node;

	if (! i->i_disabled
	    && (p->t_address == a || p->t_address == NO_ADDR)) {
		switch(p->t_oper) {
		case OP_TRACE:
			if ((! stopping && mess_type != END_SS)
			    || p->t_args[2] || ! may_stop) {
				perform(p, a);
			}
			break;
		case OP_WHEN:
			perform(p, a);
			break;
		case OP_STOP:
		case OP_DUMP:
		case OP_DISPLAY:
			break;
		default:
			assert(0);
		}
	}
  }
  return stopping;
}

handle_displays()
{
  register p_item i = item_list.il_first;

  while (i) {
	register p_tree p = i->i_node;

	if (! i->i_disabled && p->t_oper == OP_DISPLAY) do_print(p);
	i = i->i_next;
  }
}

add_to_item_list(p)
  p_tree	p;
{
  p_item i;
  
  if (in_item_list(p)) return 1;

  i = new_item();
  i->i_node = p;
  if (p->t_address == NO_ADDR
      && (p->t_oper != OP_TRACE || ! p->t_args[0])) db_ss++;
  if (item_list.il_first == 0) {
	item_list.il_first = i;
  }
  else {
	item_list.il_last->i_next = i;
  }
  i->i_itemno = ++item_list.il_count;
  item_list.il_last = i;
  pr_item(i);
  return 1;
}

p_tree
remove_from_item_list(n)
  int	n;
{
  register p_item i = item_list.il_first, prev = 0;
  p_tree	p = 0;

  while (i) {
	if (i->i_itemno == n) break;
	prev = i;
	i = i->i_next;
  }
  if (i) {
	if (prev) {
		prev->i_next = i->i_next;
	}
	else item_list.il_first = i->i_next;
	if (i == item_list.il_last) item_list.il_last = prev;
	p = i->i_node;
	if (p->t_address == NO_ADDR
	    && (p->t_oper != OP_TRACE || ! p->t_args[0])) db_ss--;
	free_item(i);
  }
  return p;
}

p_tree
get_from_item_list(n)
  int	n;
{
  register p_item i = item_list.il_first;

  while (i) {
	if (i->i_itemno == n) return i->i_node;
	i = i->i_next;
  }
  return 0;
}

able_item(n, kind)
  int	n;
{
  register p_item i = item_list.il_first;
  register p_tree p;

  while (i) {
	if (i->i_itemno == n) break;
	i = i->i_next;
  }
  if (! i) {
	error("no item %d in current status", n);
	return;
  }
  p = i->i_node;
  if (i->i_disabled == kind) {
	warning("item %d already %sabled", n, kind ? "dis" : "en");
	return;
  }
  if (p->t_address == NO_ADDR
      && (p->t_oper != OP_TRACE || ! p->t_args[0])) {
	db_ss += kind == 1 ? (-1) : 1;
  }
  i->i_disabled = kind;
  switch(p->t_oper) {
  case OP_STOP:
  case OP_WHEN:
	setstop(p, kind ? CLRBP : SETBP);
	break;
  case OP_TRACE:
	settrace(p, kind ? CLRTRACE : SETTRACE);
	break;
  }
}

print_items()
{
  register p_item i = item_list.il_first;

  for (; i; i = i->i_next) {
	pr_item(i);
  }
}

do_items()
{
  register p_item i = item_list.il_first;

  for (; i; i = i->i_next) {
	if (! i->i_disabled && i->i_node->t_oper != OP_DUMP) eval(i->i_node);
  }
}
