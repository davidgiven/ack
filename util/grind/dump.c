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
};

/* dumping and restoring of child process.
*/
do_dump(p)
  p_tree	p;
{
  struct dump *d = (struct dump *) Malloc(sizeof(struct dump));

  if (! get_dump(&d->mglobal, &d->globals, &d->mstack, &d->stack)) {
	error("no debuggee");
	free((char *) d);
	return;
  }
  p->t_args[0] = (struct tree *) d;
  p->t_address = (t_addr) get_int(d->mglobal.m_buf+PC_OFF*pointer_size, pointer_size, T_UNSIGNED);
  add_to_item_list(p);
}

/* dumping and restoring of child process.
*/
do_restore(p)
  p_tree	p;
{
  struct dump *d;
  
  p = get_from_item_list((int) p->t_ival);
  if (!p || p->t_oper != OP_DUMP) {
	error("no such dump");
	return;
  }

  d = (struct dump *) p->t_args[0];

  if (! put_dump(&d->mglobal, d->globals, &d->mstack, d->stack)) {
	error("no debuggee");
  }
  do_items();
}

free_dump(p)
  p_tree	p;
{
  struct dump *d = (struct dump *) p->t_args[0];

  free(d->globals);
  free(d->stack);
  free((char *) d);
}
