/* $Header$ */

#include <alloc.h>
#include <stdio.h>
#include <assert.h>

#include "position.h"
#include "tree.h"
#include "operator.h"

extern FILE	*db_out;
extern int	db_ss;

typedef struct item {
  struct item		*i_next;
  struct tree		*i_node;
} t_item, *p_item;

/* STATICALLOCDEF "item" 10 */

struct itemlist {
  p_item	il_first, il_last;
  int		il_count;
};

static struct itemlist	item_list;

int
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

int
item_addr_actions(a)
  t_addr	a;
{
  /* Perform actions associated with position 'a', and return 1 if we must stop
     there, and 0 if not.
  */
  register p_item i = item_list.il_first;
  int stopping = 0;

  while (i) {
	register p_tree	p = i->i_node;

	if (p->t_address == a || p->t_address == NO_ADDR) {
		switch(p->t_oper) {
		case OP_TRACE:
		case OP_WHEN:
			if (! p->t_args[1] ||
			    eval_cond(p->t_args[1])) {
				perform(p, a);
			}
			break;
		case OP_STOP:
			if (! p->t_args[1] ||
			    eval_cond(p->t_args[1])) stopping = 1;
			break;
		case OP_DUMP:
		case OP_DISPLAY:
			break;
		default:
			assert(0);
		}
	}
	i = i->i_next;
  }
  return stopping;
}

handle_displays()
{
  register p_item i = item_list.il_first;

  while (i) {
	register p_tree p = i->i_node;

	if (p->t_oper == OP_DISPLAY) do_print(p);
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
  if (p->t_address == NO_ADDR &&
      (p->t_oper != OP_TRACE || ! p->t_args[0])) db_ss++;
  if (item_list.il_first == 0) {
	item_list.il_first = i;
  }
  else {
	item_list.il_last->i_next = i;
  }
  p->t_itemno = ++item_list.il_count;
  item_list.il_last = i;
  pr_item(p);
  return 1;
}

p_tree
remove_from_item_list(n)
  int	n;
{
  register p_item i = item_list.il_first, prev = 0;
  p_tree	p = 0;

  while (i) {
	if (i->i_node->t_itemno == n) break;
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
	if (p->t_address == NO_ADDR &&
	    (p->t_oper != OP_TRACE || ! p->t_args[0])) db_ss--;
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
	if (i->i_node->t_itemno == n) return i->i_node;
	i = i->i_next;
  }
  return 0;
}

print_items()
{
  register p_item i = item_list.il_first;

  for (; i; i = i->i_next) {
	pr_item(i->i_node);
  }
}

pr_item(p)
  p_tree	p;
{
  fprintf(db_out, "(%d)\t", p->t_itemno);
  print_node(p, 1);
}

do_items()
{
  register p_item i = item_list.il_first;

  for (; i; i = i->i_next) {
	if (i->i_node->t_oper != OP_DUMP) eval(i->i_node);
  }
}
