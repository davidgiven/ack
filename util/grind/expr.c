/* $Header$ */

/* This file contains the expression evaluator. It exports the following
   routines:
   - int eval_cond(p_tree p)
	This routine evaluates the conditional expression indicated by p
	and returns 1 if it evaluates to TRUE, or 0 if it could not be
	evaluated for some reason or if it evalutes to FALSE.
	If the expression cannot be evaluated, an error message is given.
   - int eval_desig(p_tree p, t_addr *paddr, long **psize, p_type *ptp)
	This routine evaluates the expression indicated by p, which should
	result in a designator. The result of the expression is an address
	which is to be found in *paddr. *psize will contain the size of the
	designated object, and *ptp its type.
	If the expression cannot be evaluated or does not result in a
	designator, 0 is returned and an error message is given.
	Otherwise, 1 is returned.
   - int eval_expr(p_tree p, char **pbuf, long **psize, p_type *ptp)
	This routine evaluates the expression indicated by p.
	The result of the expression is left in *pbuf.
	*psize will contain the size of the value, and *ptp its type.
	If the expression cannot be evaluated, 0 is returned and an error
	message is given.  Otherwise, 1 is returned.
   - int convert(char **pbuf, long *psize, p_type *ptp, p_type tp, long size)
	This routine tries to convert the value in pbuf of size psize
	and type ptp to type tp with size size. It returns 0 if this fails,
	while producing an error message. Otherwise, it returns 1 and
	the resulting value, type and size are left in pbuf, ptp, and
	psize, respectively.
   - long get_int(char *buf, long size, int class)
	Returns the value of size 'size', residing in 'buf', of 'class'
	T_INTEGER, T_UNSIGNED, or T_ENUM.
   - int put_int(char *buf, long size, long value)
	Stores the value 'value' of size 'size' in 'buf'.
   - double get_real(char *buf, long size)
	Returns the real value of size 'size', residing in 'buf'.
	T_INTEGER, T_UNSIGNED, or T_ENUM.
   - int put_real(char *buf, long size, double value)
	Stores the value 'value' of size 'size' in 'buf'.
*/

#include <stdio.h>
#include <alloc.h>
#include <assert.h>

#include "position.h"
#include "operator.h"
#include "tree.h"
#include "expr.h"
#include "symbol.h"
#include "type.h"
#include "langdep.h"
#include "scope.h"
#include "idf.h"
#include "misc.h"

extern FILE	*db_out;
extern int	stack_offset;
extern char	*strcpy();
extern t_addr	*get_EM_regs();
extern char	*memcpy();
extern char	*malloc(), *realloc();

#define	malloc_succeeded(p)	if (! (p)) {\
					error("could not allocate enough memory");\
					return 0;\
				}

/* static t_addr	get_addr(p_symbol sym; long *psize);
   Get the address of the object indicated by sym. Returns 0 on failure,
   address on success. *psize will contain size of object.
   For local variables or parameters, the 'stack_offset' variable is
   used to determine from which stack frame the search must start.
*/
static t_addr
get_addr(sym, psize)
  register p_symbol	sym;
  long			*psize;
{
  p_type	tp = sym->sy_type;
  long		size = tp->ty_size;
  t_addr	*EM_regs;
  int		i;
  p_scope	sc, symsc;

  *psize = size;
  switch(sym->sy_class) {
  case VAR:
	/* exists if child exists; nm_value contains addres */
	return (t_addr) sym->sy_name.nm_value;
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
			return 0;
		}
		if (! EM_regs[1]) {
			error("%s not available", sym->sy_idf->id_text);
			return 0;
		}
		sc = base_scope(get_scope_from_addr(EM_regs[2]));
		if (! sc || sc->sc_start > EM_regs[2]) {
			error("%s not available", sym->sy_idf->id_text);
			sc = 0;
			return 0;
		}
		if (sc == symsc) break;		/* found it */
	}

	if (sym->sy_class == LOCVAR) {
		/* Either local variable or value parameter */
		return EM_regs[sym->sy_name.nm_value < 0 ? 0 : 1] +
				  (t_addr) sym->sy_name.nm_value;
	}

	/* If we get here, we have a var parameter. Get the parameters
	   of the current procedure invocation.
	*/
	{
		p_type proctype = sc->sc_definedby->sy_type;
		t_addr a;
		char *AB;

		size = proctype->ty_nbparams;
		if (has_static_link(sc)) size += pointer_size;
		AB = malloc((unsigned) size);
		if (! AB) {
			error("could not allocate enough memory");
			break;
		}
		if (! get_bytes(size, EM_regs[1], AB)) {
			break;
		}
		if ((size = tp->ty_size) == 0) {
			size = compute_size(tp, AB);
			*psize = size;
		}
		a = (t_addr) get_int(AB+sym->sy_name.nm_value, pointer_size, T_UNSIGNED);
		free(AB);
		return a;
	}
  default:
	error("%s is not a variable", sym->sy_idf->id_text);
	break;
  }
  return 0;
}

static int
get_v(a, pbuf, size)
  t_addr	a;
  char		**pbuf;
  long		size;
{
  if (a) {
	*pbuf = malloc((unsigned) size);
	if (! *pbuf) {
		error("could not allocate enough memory");
		return 0;
	}
	if (! get_bytes(size, a, *pbuf)) return 0;
  	return 1;
  }
  return 0;
}

/* static int	get_value(p_symbol sym; char **pbuf; long *psize);
   Get the value of the symbol indicated by sym.  Return 0 on failure,
   1 on success. On success, 'pbuf' contains the value, and 'psize' contains
   the size. For 'pbuf', storage is allocated by malloc; this storage must
   be freed by caller (I don't like this any more than you do, but caller
   does not know sizes).
   For local variables or parameters, the 'stack_offset' variable is
   used to determine from which stack frame the search must start.
*/
static int
get_value(sym, pbuf, psize)
  register p_symbol	sym;
  char	**pbuf;
  long	*psize;
{
  p_type	tp = sym->sy_type;
  int		retval = 0;
  t_addr	a;
  long		size = tp->ty_size;

  *pbuf = 0;
  switch(sym->sy_class) {
  case CONST:
	*pbuf = malloc((unsigned) size);
	if (! *pbuf) {
		error("could not allocate enough memory");
		break;
	}
	switch(tp->ty_class) {
	case T_REAL:
		put_real(*pbuf, size, sym->sy_const.co_rval);
		break;
	case T_INTEGER:
	case T_SUBRANGE:
	case T_UNSIGNED:
	case T_ENUM:
		put_int(*pbuf, size, sym->sy_const.co_ival);
		break;
	case T_SET:
		memcpy(*pbuf, sym->sy_const.co_setval, (int) size);
		break;
	case T_STRING:
		memcpy(*pbuf, sym->sy_const.co_sval, (int) size);
		break;
	default:
		fatal("strange constant");
	}
	retval = 1;
	break;
  case VAR:
  case VARPAR:
  case LOCVAR:
	a = get_addr(sym, psize);
	retval = get_v(a, pbuf, *psize);
	size = *psize;
	break;
  case UBOUND:
	a = get_addr(sym->sy_descr, psize);
	retval = get_v(a, pbuf, *psize);
	if (! retval) break;
	size = get_int(*pbuf, *psize, T_INTEGER);
	retval = get_v(a+*psize, pbuf, *psize);
	if (! retval) break;
	size += get_int(*pbuf, *psize, T_INTEGER);
	put_int(*pbuf, *psize, size);
	size = *psize;
	break;
  case LBOUND:
	a = get_addr(sym->sy_descr, psize);
	retval = get_v(a, pbuf, *psize);
	break;
  }

  if (retval == 0) {
	if (*pbuf) free(*pbuf);
	*pbuf = 0;
	*psize = 0;
  }
  else *psize = size;

  return retval;
}

/* buffer to integer and vice versa routines */

long
get_int(buf, size, class)
  char	*buf;
  long	size;
  int	class;
{
  register long l;

  switch((int)size) {
  case sizeof(char):
	l = *buf;
	if (class == T_INTEGER && l >= 0x7F) l -= 256;
	else if (class != T_INTEGER && l < 0) l += 256;
	break;
  case sizeof(short):
	l = *((short *) buf);
	if (class == T_INTEGER && l >= 0x7FFF) l -= 65536;
	else if (class != T_INTEGER && l < 0) l += 65536;
	break;
  default:
	l = *((long *) buf);
  }
  return l;
}

put_int(buf, size, value)
  char	*buf;
  long	size;
  long	value;
{
  switch((int)size) {
  case sizeof(char):
	*buf = value;
	break;
  case sizeof(short):
	*((short *) buf) = value;
	break;
  default:
	*((long *) buf) = value;
	break;
  }
  /*NOTREACHED*/
}

/* buffer to real and vice versa routines */

double
get_real(buf, size)
  char	*buf;
  long	size;
{
  switch((int) size) {
  case sizeof(float):
	return *((float *) buf);
  default:
	return *((double *) buf);
  }
  /*NOTREACHED*/
}

put_real(buf, size, value)
  char	*buf;
  long	size;
  double value;
{
  switch((int)size) {
  case sizeof(float):
	*((float *) buf) = value;
	break;
  default:
	*((double *) buf) = value;
	break;
  }
  /* NOTREACHED */
}

int
convert(pbuf, psize, ptp, tp, size)
  char	**pbuf;
  long	*psize;
  register p_type *ptp;
  register p_type tp;
  long size;
{
  /* Convert the value in pbuf, of size psize and type ptp, to type
     tp and leave the resulting value in pbuf, the resulting size
     in psize, and the resulting type in ptp.
  */
  long	l;
  double d;

  if (*ptp == tp) return 1;
  if (size > *psize) {
	*pbuf = realloc(*pbuf, (unsigned int) size);
	malloc_succeeded(*pbuf);
  }
  if ((*ptp)->ty_class == T_SUBRANGE) *ptp = (*ptp)->ty_base;
  if (tp && *ptp) switch((*ptp)->ty_class) {
  case T_INTEGER:
  case T_UNSIGNED:
  case T_POINTER:
  case T_ENUM:
	l = get_int(*pbuf, *psize, (*ptp)->ty_class);
	if (tp == bool_type) l = l != 0;
	switch(tp->ty_class) {
  	case T_SUBRANGE:
  	case T_INTEGER:
  	case T_UNSIGNED:
  	case T_POINTER:
  	case T_ENUM:
		put_int(*pbuf, size, l);
		*psize = size;
		*ptp = tp;
		return 1;
	case T_REAL:
		put_real(*pbuf,
			 size,
			 (*ptp)->ty_class == T_INTEGER 
				? (double) l
				: (double) (unsigned long) l);
		*psize = size;
		*ptp = tp;
		return 1;
	default:
		break;
	}
	break;
  case T_REAL:
	d = get_real(*pbuf, *psize);
	switch(tp->ty_class) {
  	case T_ENUM:
  	case T_SUBRANGE:
  	case T_INTEGER:
  	case T_UNSIGNED:
  	case T_POINTER:
		if (tp == bool_type) put_int(*pbuf, size, (long) (d != 0));
		else put_int(*pbuf, size, (long) d);
		*psize = size;
		*ptp = tp;
		return 1;
	case T_REAL:
		put_real(*pbuf, size, d);
		*psize = size;
		*ptp = tp;
		return 1;
	default:
		break;
	}
	break;
  default:
	break;
  }
  error("illegal conversion");
  return 0;
}

int
eval_cond(p)
  p_tree	p;
{
  char	*buf;
  long	size;
  p_type tp;
  long val;
  p_type target_tp = currlang->has_bool_type ? bool_type : int_type;

  if (eval_expr(p, &buf, &size, &tp)) {
	if (convert(&buf, &size, &tp, target_tp, target_tp->ty_size)) {
		val = get_int(buf, size, T_UNSIGNED);
		free(buf);
		return (int) (val != 0);
	}
	free(buf);
  }
  return 0;
}

/* one routine for each unary operator */

static int
not_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  p_type target_tp = currlang->has_bool_type ? bool_type : int_type;

  if (eval_expr(p->t_args[0], pbuf, psize, ptp) &&
      convert(pbuf, psize, ptp, target_tp, target_tp->ty_size)) {
	put_int(*pbuf, *psize, (long) !get_int(*pbuf, *psize, T_UNSIGNED));
	return 1;
  }
  return 0;
}

static int
bnot_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  if (eval_expr(p->t_args[0], pbuf, psize, ptp)) {
	switch((*ptp)->ty_class) {
	case T_INTEGER:
	case T_ENUM:
	case T_UNSIGNED:
	case T_SUBRANGE:
		put_int(*pbuf, *psize, ~get_int(*pbuf, *psize, T_UNSIGNED));
		return 1;
	default:
		error("illegal operand type(s)");
		break;
	}
  }
  return 0;
}

static int
ptr_addr(p, paddr, psize, ptp)
  p_tree	p;
  t_addr	*paddr;
  long		*psize;
  p_type	*ptp;
{
  char	*buf;

  if (eval_expr(p->t_args[0], &buf, psize, ptp)) {
	switch((*ptp)->ty_class) {
	case T_POINTER:
		*ptp = (*ptp)->ty_ptrto;
		*psize = (*ptp)->ty_size;
		*paddr = get_int(buf, pointer_size, T_UNSIGNED);
		free(buf);
		return 1;
  	default:
		error("illegal operand of DEREF");
		free(buf);
		break;
	}
  }
  return 0;
}

static int
deref_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  t_addr addr;

  if (ptr_addr(p, &addr, psize, ptp)) {
	*pbuf = malloc((unsigned) *psize);
	malloc_succeeded(*pbuf);
	if (! get_bytes(*psize, addr, *pbuf)) {
		free(*pbuf);
		*pbuf = 0;
		return 0;
	}
	return 1;
  }
  return 0;
}

static int
addr_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  t_addr addr;

  if (eval_desig(p->t_args[0], &addr, psize, ptp)) {
	*pbuf = malloc((unsigned) pointer_size);
	malloc_succeeded(*pbuf);
	put_int(*pbuf, pointer_size, (long) addr);
	address_type->ty_ptrto = *ptp;
	*ptp = address_type;
	return 1;
  }
  return 0;
}

static int
unmin_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  if (eval_expr(p->t_args[0], pbuf, psize, ptp)) {
  	switch((*ptp)->ty_class) {
  	case T_SUBRANGE:
  	case T_INTEGER:
  	case T_ENUM:
  	case T_UNSIGNED:
		put_int(*pbuf, *psize, -get_int(*pbuf, *psize, (*ptp)->ty_class));
		return 1;
  	case T_REAL:
		put_real(*pbuf, *psize, -get_real(*pbuf, *psize));
		return 1;
  	default:
		error("illegal operand of unary -");
		break;
	}
  }
  return 0;
}

static int
unplus_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  if (eval_expr(p->t_args[0], pbuf, psize, ptp)) {
  	switch((*ptp)->ty_class) {
  	case T_SUBRANGE:
  	case T_INTEGER:
  	case T_ENUM:
  	case T_UNSIGNED:
  	case T_REAL:
		return 1;
  	default:
		error("illegal operand of unary +");
		break;
  	}
  }
  return 0;
}

static int (*un_op[])() = {
  0,
  not_op,
  deref_op,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  unplus_op,
  unmin_op,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  bnot_op,
  0,
  0,
  0,
  addr_op
};

static p_type
balance(tp1, tp2)
  p_type	tp1, tp2;
{

  if (tp1->ty_class == T_SUBRANGE) tp1 = tp1->ty_base;
  if (tp2->ty_class == T_SUBRANGE) tp2 = tp2->ty_base;
  if (tp1 == tp2) return tp2;
  if (tp2->ty_class == T_REAL) {
  	p_type tmp = tp1; tp1 = tp2; tp2 = tmp;
  }
  if (tp1->ty_class == T_REAL) {
	switch(tp2->ty_class) {
	case T_INTEGER:
	case T_UNSIGNED:
	case T_ENUM:
		return tp1;
	case T_REAL:
		return tp1->ty_size > tp2->ty_size ? tp1 : tp2;
	default:
		error("illegal type combination");
		return 0;
	}
  }
  if (tp2->ty_class == T_POINTER) {
  	p_type tmp = tp1; tp1 = tp2; tp2 = tmp;
  }
  if (tp1->ty_class == T_POINTER) {
	switch(tp2->ty_class) {
	case T_INTEGER:
	case T_UNSIGNED:
	case T_POINTER:
	case T_ENUM:
		return tp1;
	default:
		error("illegal type combination");
		return 0;
	}
  }
  if (tp2->ty_class == T_UNSIGNED) {
  	p_type tmp = tp1; tp1 = tp2; tp2 = tmp;
  }
  if (tp1->ty_class == T_UNSIGNED) {
	switch(tp2->ty_class) {
	case T_INTEGER:
	case T_UNSIGNED:
		if (tp1->ty_size >= tp2->ty_size) return tp1;
		return tp2;
	case T_ENUM:
		return tp1;
	default:
		error("illegal type combination");
		return 0;
	}
  }
  if (tp2->ty_class == T_INTEGER) {
  	p_type tmp = tp1; tp1 = tp2; tp2 = tmp;
  }
  if (tp1->ty_class == T_INTEGER) {
	switch(tp2->ty_class) {
	case T_INTEGER:
		if (tp1->ty_size >= tp2->ty_size) return tp1;
		return tp2;
	case T_ENUM:
		return tp1;
	default:
		error("illegal type combination");
		return 0;
	}
  }
  error("illegal type combination");
  return 0;
}

static int
andor_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  long		l1, l2;
  char		*buf = 0;
  long		size;
  p_type	tp;
  p_type	target_tp = currlang->has_bool_type ? bool_type : int_type;

  if (eval_expr(p->t_args[0], pbuf, psize, ptp) &&
      convert(pbuf, psize, ptp, target_tp, target_tp->ty_size) &&
      eval_expr(p->t_args[1], &buf, &size, &tp) &&
      convert(&buf, &size, &tp, target_tp, target_tp->ty_size)) {
	l1 = get_int(*pbuf, *psize, T_UNSIGNED);
	l2 = get_int(buf, size, T_UNSIGNED);
	put_int(*pbuf,
		*psize,
		p->t_whichoper == E_AND 
			? (long)(l1 && l2) 
			: (long)(l1 || l2));
	free(buf);
	return 1;
  }
  if (buf) free(buf);
  return 0;
}

static int
arith_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  long		l1, l2;
  double	d1, d2;
  char		*buf = 0;
  long		size;
  p_type	tp, balance_tp;

  if (!(eval_expr(p->t_args[0], pbuf, psize, ptp) &&
        eval_expr(p->t_args[1], &buf, &size, &tp))) {
	return 0;
  }
  if ((*ptp)->ty_class == T_POINTER) {
	if (currlang != c_dep ||
	    (p->t_whichoper != E_PLUS && p->t_whichoper != E_MIN)) {
		error("illegal operand type(s)");
		free(buf);
		return 0;
	}
	l1 = get_int(*pbuf, *psize, T_UNSIGNED);
	if (tp->ty_class == T_POINTER) {
		if (p->t_whichoper != E_MIN) {
			error("illegal operand type(s)");
			free(buf);
			return 0;
		}
		l2 = get_int(buf, size, T_UNSIGNED);
		free(buf);
		*pbuf = Realloc(*pbuf, (unsigned) long_size);
		put_int(*pbuf, long_size, (l1 - l2)/(*ptp)->ty_ptrto->ty_size);
		*ptp = long_type;
		return 1;
	}
	if (! convert(&buf, &size, &tp, long_type, long_size)) {
		free(buf);
		return 0;
	}
	l2 = get_int(buf, size, T_INTEGER) * (*ptp)->ty_ptrto->ty_size;
	free(buf);
	buf = 0;
	if (p->t_whichoper == E_PLUS) l1 += l2;
	else l1 -= l2;
	put_int(*pbuf, *psize, l1);
	return 1;
  }
  if ((balance_tp = balance(*ptp, tp)) &&
      convert(pbuf, psize, ptp, balance_tp, balance_tp->ty_size) &&
      convert(&buf, &size, &tp, balance_tp, balance_tp->ty_size)) {
	switch(balance_tp->ty_class) {
	case T_INTEGER:
	case T_ENUM:
	case T_UNSIGNED:
		l1 = get_int(*pbuf, *psize, balance_tp->ty_class);
		l2 = get_int(buf, size, balance_tp->ty_class);
		free(buf);
		buf = 0;
		switch(p->t_whichoper) {
		case E_BAND:
			l1 &= l2;
			break;
		case E_BOR:
			l1 |= l2;
			break;
		case E_BXOR:
			l1 ^= l2;
			break;
		case E_PLUS:
			l1 += l2;
			break;
		case E_MIN:
			l1 -= l2;
			break;
		case E_MUL:
			l1 *= l2;
			break;
		case E_DIV:
		case E_ZDIV:
			if (! l2) {
				error("division by 0");
				return 0;
			}
			if (balance_tp->ty_class == T_INTEGER) {
				if ((l1 < 0) != (l2 < 0)) {
					if (l1 < 0) l1 = - l1;
					else l2 = -l2;
					if (p->t_whichoper == E_DIV) {
					    l1 = -((l1+l2-1)/l2);
					}
					else {
					    l1 = -(l1/l2);
					}
				}
				else l1 /= l2;
			}
			else l1 = (unsigned long) l1 /
				  (unsigned long) l2;
			break;
		case E_MOD:
		case E_ZMOD:
			if (! l2) {
				error("modulo by 0");
				return 0;
			}
			if (balance_tp->ty_class == T_INTEGER) {
				if ((l1 < 0) != (l2 < 0)) {
					if (l1 < 0) l1 = - l1;
					else l2 = -l2;
					if (p->t_whichoper == E_MOD) {
					    l1 = ((l1+l2-1)/l2)*l2 - l1;
					}
					else {
					    l1 = (l1/l2)*l2 - l1;
					}
				}
				else l1 %= l2;
			}
			else l1 = (unsigned long) l1 %
				  (unsigned long) l2;
			break;
		}
		put_int(*pbuf, *psize, l1);
		break;
	case T_REAL:
		d1 = get_real(*pbuf, *psize);
		d2 = get_real(buf, size);
		free(buf);
		buf = 0;
		switch(p->t_whichoper) {
		case E_DIV:
		case E_ZDIV:
			if (d2 == 0.0) {
				error("division by 0.0");
				return 0;
			}
			d1 /= d2;
			break;
		case E_PLUS:
			d1 += d2;
			break;
		case E_MIN:
			d1 -= d2;
			break;
		case E_MUL:
			d1 *= d2;
			break;
		}
		put_real(*pbuf, *psize, d1);
		break;
	default:
		error("illegal operand type(s)");
		free(buf);
		return 0;
	}
	return 1;
  }
  if (buf) free(buf);
  return 0;
}

static int
sft_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  long		l1, l2;
  char		*buf = 0;
  long		size;
  p_type	tp;

  if (eval_expr(p->t_args[0], pbuf, psize, ptp) &&
      eval_expr(p->t_args[1], &buf, &size, &tp) &&
      convert(&buf, &size, &tp, int_type, int_size)) {
	tp = *ptp;
	if (tp->ty_class == T_SUBRANGE) {
		tp = tp->ty_base;
	}
	switch(tp->ty_class) {
	case T_INTEGER:
	case T_ENUM:
	case T_UNSIGNED:
		l1 = get_int(*pbuf, *psize, tp->ty_class);
		l2 = get_int(buf, size, T_INTEGER);
		free(buf);
		buf = 0;
		switch(p->t_whichoper) {
		case E_LSFT:
			l1 <<= (int) l2;
			break;
		case E_RSFT:
			if (tp->ty_class == T_INTEGER) l1 >>= (int) l2;
			else l1 = (unsigned long) l1 >> (int) l2;
			break;
		}
		break;
	default:
		error("illegal operand type(s)");
		free(buf);
		return 0;
	}
	return 1;
  }
  if (buf) free(buf);
  return 0;
}

static int
cmp_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  long		l1, l2;
  double	d1, d2;
  char		*buf = 0;
  long		size;
  p_type	tp, balance_tp;

  if (eval_expr(p->t_args[0], pbuf, psize, ptp) &&
      eval_expr(p->t_args[1], &buf, &size, &tp) &&
      (balance_tp = balance(*ptp, tp)) &&
      convert(pbuf, psize, ptp, balance_tp, balance_tp->ty_size) &&
      convert(&buf, &size, &tp, balance_tp, balance_tp->ty_size)) {
	switch(balance_tp->ty_class) {
	case T_INTEGER:
	case T_ENUM:
	case T_UNSIGNED:
	case T_POINTER:
		l1 = get_int(*pbuf, *psize, balance_tp->ty_class);
		l2 = get_int(buf, size, balance_tp->ty_class);
		free(buf);
		buf = 0;
		switch(p->t_whichoper) {
		case E_EQUAL:
			l1 = l1 == l2;
			break;
		case E_NOTEQUAL:
			l1 = l1 != l2;
			break;
		case E_LTEQUAL:
			if (balance_tp->ty_class == T_INTEGER) {
				l1 = l1 <= l2;
			}
			else	l1 = (unsigned long) l1 <=
				     (unsigned long) l2;
			break;
		case E_LT:
			if (balance_tp->ty_class == T_INTEGER) {
				l1 = l1 < l2;
			}
			else	l1 = (unsigned long) l1 <
				     (unsigned long) l2;
			break;
		case E_GTEQUAL:
			if (balance_tp->ty_class == T_INTEGER) {
				l1 = l1 >= l2;
			}
			else	l1 = (unsigned long) l1 >=
				     (unsigned long) l2;
			break;
		case E_GT:
			if (balance_tp->ty_class == T_INTEGER) {
				l1 = l1 > l2;
			}
			else	l1 = (unsigned long) l1 >
				     (unsigned long) l2;
			break;
		default:
			l1 = 0;
			assert(0);
			break;
		}
		break;
	case T_REAL:
		d1 = get_real(*pbuf, *psize);
		d2 = get_real(buf, size);
		free(buf);
		buf = 0;
		switch(p->t_whichoper) {
		case E_EQUAL:
			l1 = d1 == d2;
			break;
		case E_NOTEQUAL:
			l1 = d1 != d2;
			break;
		case E_LTEQUAL:
			l1 = d1 <= d2;
			break;
		case E_LT:
			l1 = d1 < d2;
			break;
		case E_GTEQUAL:
			l1 = d1 >= d2;
			break;
		case E_GT:
			l1 = d1 > d2;
			break;
		default:
			l1 = 0;
			assert(0);
			break;
		}
		break;
	default:
		error("illegal operand type(s)");
		free(buf);
		return 0;
	}
	if (*psize < int_size) {
		*psize = int_size;
		*pbuf = realloc(*pbuf, (unsigned int) int_size);
		malloc_succeeded(*pbuf);
	}
	else	*psize = int_size;
	if (currlang->has_bool_type) {
		*ptp = bool_type;
	}
	else	*ptp = int_type;
	put_int(*pbuf, *psize, l1);
	return 1;
  }
  if (buf) free(buf);
  return 0;
}

static int
in_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  long		l;
  char		*buf = 0;
  long		size;
  p_type	tp;
  int		sft = int_size == 2 ? 4 : 5;

  if (eval_expr(p->t_args[0], pbuf, psize, ptp) &&
      eval_expr(p->t_args[1], &buf, &size, &tp)) {
	if (tp->ty_class != T_SET) {
		error("right-hand side of IN not a set");
		free(buf);
		return 0;
	}
	if (! convert(pbuf, psize, ptp, tp->ty_setbase, int_size)) {
		free(buf);
		return 0;
	}
	l = get_int(*pbuf, *psize, (*ptp)->ty_class) - tp->ty_setlow;
	l = l >= 0 
	    && l <= (size << 3) 
	    && (((int *) buf)[(int)(l>>sft)] & (1 << (l & ((1 << sft)-1))));
	free(buf);
	*pbuf = realloc(*pbuf, (unsigned) int_size);
	malloc_succeeded(*pbuf);
	*psize = int_size;
	*ptp = currlang->has_bool_type ? bool_type : int_type;
	put_int(*pbuf, *psize, l);
	return 1;
  }
  return 0;
}

static int
array_addr(p, paddr, psize, ptp)
  p_tree	p;
  t_addr	*paddr;
  long		*psize;
  p_type	*ptp;
{
  long		l;
  char		*buf = 0;
  long		size;
  p_type	tp;

  if (eval_desig(p->t_args[0], paddr, psize, ptp) &&
      eval_expr(p->t_args[1], &buf, &size, &tp)) {
	if ((*ptp)->ty_class != T_ARRAY && (*ptp)->ty_class != T_POINTER) {
		error("illegal left-hand side of [");
		free(buf);
		return 0;
	}
	if ((*ptp)->ty_class == T_POINTER) {
		if (! get_bytes(pointer_size, *paddr, (char *) paddr)) {
			free(buf);
			return 0;
		}
		*paddr = get_int((char *) paddr, pointer_size, T_UNSIGNED);
	}
	if (! convert(&buf, &size, &tp, int_type, int_size)) {
		free(buf);
		return 0;
	}
	l = get_int(buf, size, T_INTEGER);
	free(buf);
	buf = 0;
	if ((*ptp)->ty_class == T_ARRAY) {
	    	if (l < (*ptp)->ty_lb || l > (*ptp)->ty_hb) {
			error("array bound error");
			return 0;
		}
		l -= (*ptp)->ty_lb;
		*ptp = (*ptp)->ty_elements;
		l *= (*currlang->arrayelsize)((*ptp)->ty_size);
	}
	else {
		*ptp = (*ptp)->ty_ptrto;
		l *= (*ptp)->ty_size;
	}
	*psize = (*ptp)->ty_size;
	*paddr += l;
	return 1;
  }
  return 0;
}

static int
array_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  t_addr	a;

  if (array_addr(p, &a, psize, ptp)) {
	*pbuf = malloc((unsigned int) *psize);
	malloc_succeeded(*pbuf);
	if (! get_bytes(*psize, a, *pbuf)) {
		return 0;
	}
	return 1;
  }
  return 0;
}

static int
select_addr(p, paddr, psize, ptp)
  p_tree	p;
  t_addr	*paddr;
  long		*psize;
  p_type	*ptp;
{
  register p_type	tp;
  register struct fields *f;
  register int		nf;

  if (eval_desig(p->t_args[0], paddr, psize, ptp)) {
	tp = *ptp;
	if (tp->ty_class != T_STRUCT && tp->ty_class != T_UNION) {
		error("SELECT on non-struct");
		return 0;
	}
	if (p->t_args[1]->t_oper != OP_NAME) {
		error("right-hand side of SELECT not a name");
		return 0;
	}
	for (nf = tp->ty_nfields, f = tp->ty_fields; nf; nf--, f++) {
		if (! strcmp(f->fld_name, p->t_args[1]->t_str)) break;
	}
	if (! nf) {
		error("'%s' not found", p->t_args[1]->t_str);
		return 0;
	}
	
	/* ??? this needs some work for bitfields ??? */
	*paddr += f->fld_pos>>3;
	*psize = f->fld_bitsize >> 3;
	*ptp = f->fld_type;
	return 1;
  }
  return 0;
}

static int
select_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  t_addr	a;
  if (select_addr(p, &a, psize, ptp)) {
	*pbuf = malloc((unsigned int) *psize);
	malloc_succeeded(*pbuf);
	if (! get_bytes(*psize, a, *pbuf)) {
		free(*pbuf);
		*pbuf = 0;
		return 0;
	}
	return 1;
  }
  return 0;
}

static int
derselect_op(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  int	retval;
  t_tree	t;

  t.t_oper = OP_UNOP;
  t.t_whichoper = E_DEREF;
  t.t_args[0] = p->t_args[0];
  p->t_args[0] = &t;
  p->t_whichoper = E_SELECT;
  retval = eval_expr(p, pbuf, psize, ptp);
  p->t_args[0] = t.t_args[0];
  p->t_whichoper = E_DERSELECT;
  return retval;
}

static int (*bin_op[])() = {
  0,
  0,
  0,
  andor_op,
  andor_op,
  arith_op,
  arith_op,
  arith_op,
  arith_op,
  in_op,
  array_op,
  arith_op,
  arith_op,
  arith_op,
  cmp_op,
  cmp_op,
  cmp_op,
  cmp_op,
  cmp_op,
  cmp_op,
  select_op,
  arith_op,
  arith_op,
  arith_op,
  0,
  derselect_op,
  sft_op,
  sft_op,
  0
};

int
eval_expr(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  register p_symbol	sym;
  int		retval = 0;

  *pbuf = 0;

  switch(p->t_oper) {
  case OP_FORMAT:
	if (eval_expr(p->t_args[0], pbuf, psize, ptp)) retval = 1;
	break;
  case OP_NAME:
  case OP_SELECT:
	sym = identify(p, VAR|REGVAR|LOCVAR|VARPAR|CONST|LBOUND|UBOUND);
	if (! sym) return 0;
	if (! get_value(sym, pbuf, psize)) {
		break;
	}
	*ptp = sym->sy_type;
	retval = 1;
	break;

  case OP_INTEGER:
	*pbuf = malloc((unsigned int) long_size);
	malloc_succeeded(*pbuf);
	*psize = long_size;
	*ptp = long_type;
	put_int(*pbuf, long_size, p->t_ival);
	retval = 1;
	break;

  case OP_REAL:
	*pbuf = malloc((unsigned int) double_size);
	malloc_succeeded(*pbuf);
	*psize = double_size;
	*ptp = double_type;
	put_real(*pbuf, double_size, p->t_fval);
	retval = 1;
	break;

  case OP_STRING:
	*psize = strlen(p->t_sval)+1;
	*pbuf = malloc((unsigned int)*psize);
	malloc_succeeded(*pbuf);
	*ptp = string_type;
	strcpy(*pbuf, p->t_sval);
	retval = 1;
	break;

  case OP_UNOP:
	retval = (*un_op[p->t_whichoper])(p, pbuf, psize, ptp);
	break;

  case OP_BINOP:
	retval = (*bin_op[p->t_whichoper])(p, pbuf, psize, ptp);
	break;
  default:
	assert(0);
	break;
  }
  if (! retval) {
	if (*pbuf) {
		free(*pbuf);
		*pbuf = 0;
	}
	*psize = 0;
  }
  else {
	if ((*ptp)->ty_class == T_CROSS) {
		*ptp = (*ptp)->ty_cross;
		if (! *ptp) *ptp = void_type;
	}
  }
  return retval;
}

int
eval_desig(p, paddr, psize, ptp)
  p_tree	p;
  t_addr	*paddr;
  long		*psize;
  p_type	*ptp;
{
  register p_symbol	sym;
  int	retval = 0;
  t_addr a;

  switch(p->t_oper) {
  case OP_NAME:
  case OP_SELECT:
	sym = identify(p, VAR|REGVAR|LOCVAR|VARPAR);
	if (! sym) return 0;
	if (! (a = get_addr(sym, psize))) {
		break;
	}
	*paddr = a;
	*ptp = sym->sy_type;
	retval = 1;
	break;

  case OP_UNOP:
	switch(p->t_whichoper) {
	case E_DEREF:
		if (ptr_addr(p, paddr, psize, ptp)) {
			retval = 1;
		}
		break;
	default:
		print_node(db_out, p, 0);
		fputs(" not a designator\n", db_out);
		break;
	}
	break;

  case OP_BINOP:
	switch(p->t_whichoper) {
	case E_ARRAY:
		if (array_addr(p, paddr, psize, ptp)) {
			retval = 1;
		}
		break;
	case E_SELECT:
		if (select_addr(p, paddr, psize, ptp)) {
			retval = 1;
		}
		break;
	default:
		print_node(db_out, p, 0);
		fputs(" not a designator\n", db_out);
		break;
	}
	break;
  default:
	error("illegal designator");
	break;
  }
  if (! retval) {
	*psize = 0;
  }
  else {
	if ((*ptp)->ty_class == T_CROSS) {
		*ptp = (*ptp)->ty_cross;
		if (! *ptp) {
			*ptp = void_type;
			print_node(db_out, p, 0);
			fputs(" designator has unknown type\n", db_out);
			retval = 0;
			*psize = 0;
		}
	}
  }
  return retval;
}
