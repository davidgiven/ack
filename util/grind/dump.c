/* $Header$ */

#include <assert.h>
#include <alloc.h>

#include "operator.h"
#include "position.h"
#include "tree.h"
#include "message.h"
#include "type.h"
#include "expr.h"

extern long	pointer_size;
extern p_tree	get_from_item_list();

struct dump {
  char	*globals, *stack;
  struct message_hdr mglobal, mstack;
  struct dump *next;
};

static struct dump	*last_dump;

/* dumping and restoring of child process.
*/
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

/* dumping and restoring of child process.
*/
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
  do_items();
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
