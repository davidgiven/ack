/* $Header$ */

#include <alloc.h>

#include "position.h"
#include "scope.h"
#include "symbol.h"
#include "type.h"
#include "message.h"

int stack_offset;		/* for up and down commands */

extern long pointer_size;
extern t_addr *get_EM_regs();
extern char *memcpy();

/* Get the value of the symbol indicated by sym.
   Return 0 on failure,
	  1 on success.
   On success, 'buf' contains the value, and 'size' contains the size.
   For 'buf', storage is allocated by Malloc; this storage must
   be freed by caller (I don't like this any more than you do, but caller
   does not know sizes).
*/
int
get_value(sym, buf, psize)
  register p_symbol	sym;
  char	**buf;
  long	*psize;
{
  p_type	tp = sym->sy_type;
  long		size = tp->ty_size;
  int		retval = 0;
  t_addr	*EM_regs;
  int		i;
  p_scope	sc, symsc;
  char		*AB;

  *buf = 0;
  switch(sym->sy_class) {
  case VAR:
	/* exists if child exists; nm_value contains addres */
	*buf = Malloc((unsigned) size);
	if (get_bytes(size, (t_addr) sym->sy_name.nm_value, *buf)) {
		retval = 1;
	}
	break;
  case CONST:
	*buf = Malloc((unsigned) tp->ty_size);
	switch(tp->ty_class) {
	case T_REAL:
		if (tp->ty_size != sizeof(double)) {
			*((float *) *buf) = sym->sy_const.co_rval;
		}
		else	*((double *) *buf) = sym->sy_const.co_rval;
		break;
	case T_INTEGER:
	case T_SUBRANGE:
	case T_UNSIGNED:
	case T_ENUM:
		if (tp->ty_size == 1) {
			*((char *) *buf) = sym->sy_const.co_ival;
		}
		else if (tp->ty_size == 2) {
			*((short *) *buf) = sym->sy_const.co_ival;
		}
		else {
			*((long *) *buf) = sym->sy_const.co_ival;
		}
		break;
	case T_SET:
		memcpy(*buf, sym->sy_const.co_setval, (int) tp->ty_size);
		break;
	case T_STRING:
		memcpy(*buf, sym->sy_const.co_sval, (int) tp->ty_size);
		break;
	default:
		fatal("strange constant");
	}
	retval = 1;
	break;
  case VARPAR:
  case LOCVAR:
	/* first find the stack frame in which it resides */
	symsc = base_scope(sym->sy_scope);

	/* now symsc contains the scope where the storage for sym is
	   allocated. Now find it on the stack of child.
	*/
	i = stack_offset;
	for (;;) {
		sc = 0;
		if (! (EM_regs = get_EM_regs(i++))) {
			/* no child? */
			break;
		}
		if (! EM_regs[AB_OFF]) {
			/* no more frames */
			break;
		}
		sc = base_scope(get_scope_from_addr(EM_regs[PC_OFF]));
		if (! sc || sc->sc_start > EM_regs[PC_OFF]) {
			sc = 0;
			break;
		}
		if (sc == symsc) break;		/* found it */
	}

	if (! sc) break;	/* not found */

	if (sym->sy_class == LOCVAR) {
		/* Either local variable or value parameter */
		*buf = Malloc((unsigned) size);
		if (get_bytes(size,
			      EM_regs[sym->sy_name.nm_value < 0 
					? LB_OFF 
					: AB_OFF
				     ] +
				  (t_addr) sym->sy_name.nm_value,
			      *buf)) {
			retval = 1;
		}
		break;
	}

	/* If we get here, we have a var parameter. Get the parameters
	   of the current procedure invocation.
	*/
	{
		p_type proctype = sc->sc_definedby->sy_type;

		size = proctype->ty_nbparams;
		if (has_static_link(sc)) size += pointer_size;
		AB = Malloc((unsigned) size);
		if (! get_bytes(size, EM_regs[AB_OFF], AB)) {
			break;
		}
		if ((size = tp->ty_size) == 0) {
			size = compute_size(tp, AB);
		}
	}
	*buf = Malloc((unsigned) size);
	*psize = size;
	if (get_bytes(size,
		      (t_addr) BUFTOA(AB+sym->sy_name.nm_value),
		      *buf)) {
		retval = 1;
	}
	free(AB);
	break;
  }

  if (retval == 0) {
	if (*buf) free(*buf);
	*buf = 0;
	*psize = 0;
  }

  return retval;
}
