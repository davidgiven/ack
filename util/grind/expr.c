/* $Header$ */

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

extern FILE	*db_out;

static long
get_int(buf, size)
  char	*buf;
  long	size;
{
  switch((int)size) {
  case 1:
	return *buf & 0xFF;
  case 2:
	return *((short *) buf) & 0xFFFF;
  default:
	return *((long *) buf);
  }
  /* NOTREACHED */
}

static double
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

static
put_int(buf, size, value)
  char	*buf;
  long	size;
  long	value;
{
  switch((int)size) {
  case 1:
	*buf = value;
	break;
  case 2:
	*((short *) buf) = value;
	break;
  default:
	*((long *) buf) = value;
	break;
  }
  /* NOTREACHED */
}

static
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

static int
convert(pbuf, psize, ptp, tp)
  char	**pbuf;
  long	*psize;
  p_type *ptp;
  p_type tp;
{
  long	l;
  double d;

  if (*ptp == tp) return 1;
  if (tp->ty_size > *psize) {
	*pbuf = Realloc(*pbuf, (unsigned int) tp->ty_size);
  }
  if ((*ptp)->ty_class == T_SUBRANGE) *ptp = (*ptp)->ty_base;
  switch((*ptp)->ty_class) {
  case T_INTEGER:
  case T_UNSIGNED:
  case T_POINTER:
  case T_ENUM:
	l = get_int(*pbuf, *psize);
	if (tp == bool_type) l = l != 0;
	switch(tp->ty_class) {
  	case T_SUBRANGE:
  	case T_INTEGER:
  	case T_UNSIGNED:
  	case T_POINTER:
  	case T_ENUM:
		put_int(*pbuf, tp->ty_size, l);
		*psize = tp->ty_size;
		*ptp = tp;
		return 1;
	case T_REAL:
		put_real(*pbuf,
			 tp->ty_size,
			 (*ptp)->ty_class == T_INTEGER 
				? (double) l
				: (double) (unsigned long) l);
		*psize = tp->ty_size;
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
		if (tp == bool_type) put_int(*pbuf, tp->ty_size, (long) (d != 0));
		else put_int(*pbuf, tp->ty_size, (long) d);
		*psize = tp->ty_size;
		*ptp = tp;
		return 1;
	case T_REAL:
		put_real(*pbuf, tp->ty_size, d);
		*psize = tp->ty_size;
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

  if (eval_expr(p, &buf, &size, &tp)) {
	if (convert(&buf, &size, &tp, currlang->has_bool_type ? bool_type : int_type)) {
		val = get_int(buf, size);
		if (buf) free(buf);
		return (int) val;
	}
	if (buf) free(buf);
  }
  return 0;
}

static int
do_not(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  if (eval_expr(p->t_args[0], pbuf, psize, ptp) &&
      convert(pbuf, psize, ptp, currlang->has_bool_type ? bool_type : int_type)) {
	put_int(*pbuf, *psize, (long) !get_int(*pbuf, *psize));
	return 1;
  }
  return 0;
}

static int
do_deref(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  char	*addr;

  if (eval_expr(p->t_args[0], pbuf, psize, ptp)) {
	switch((*ptp)->ty_class) {
	case T_POINTER:
		addr = *((char **) (*pbuf));
		free(*pbuf);
		*ptp = (*ptp)->ty_ptrto;
		*psize = (*ptp)->ty_size;
		*pbuf = Malloc((unsigned) (*ptp)->ty_size);
		if (! get_bytes(*psize, (t_addr) addr, *pbuf)) {
			error("could not get value");
			break;
		}
		return 1;
  	default:
		error("illegal operand of DEREF");
		break;
	}
  }
  return 0;
}

static int
do_unmin(p, pbuf, psize, ptp)
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
		put_int(*pbuf, *psize, -get_int(*pbuf, *psize));
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
do_unplus(p, pbuf, psize, ptp)
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
  do_not,
  do_deref,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  do_unplus,
  do_unmin,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
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
do_andor(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  long		l1, l2;
  char		*buf;
  long		size;
  p_type	tp;

  if (eval_expr(p->t_args[0], pbuf, psize, ptp) &&
      convert(pbuf, psize, ptp, currlang->has_bool_type ? bool_type : int_type) &&
      eval_expr(p->t_args[1], &buf, &size, &tp) &&
      convert(&buf, &size, &tp, currlang->has_bool_type ? bool_type : int_type)) {
	l1 = get_int(*pbuf, *psize);
	l2 = get_int(buf, size);
	put_int(*pbuf,
		*psize,
		p->t_whichoper == E_AND 
			? (long)(l1 && l2) 
			: (long)(l1 || l2));
	free(buf);
	return 1;
  }
  free(buf);
  return 0;
}

static int
do_arith(p, pbuf, psize, ptp)
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
      convert(pbuf, psize, ptp, balance_tp) &&
      convert(&buf, &size, &tp, balance_tp)) {
	switch(balance_tp->ty_class) {
	case T_INTEGER:
	case T_ENUM:
	case T_UNSIGNED:
		l1 = get_int(*pbuf, *psize);
		l2 = get_int(buf, size);
		free(buf);
		buf = 0;
		switch(p->t_whichoper) {
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
do_cmp(p, pbuf, psize, ptp)
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
      convert(pbuf, psize, ptp, balance_tp) &&
      convert(&buf, &size, &tp, balance_tp)) {
	switch(balance_tp->ty_class) {
	case T_INTEGER:
	case T_ENUM:
	case T_UNSIGNED:
	case T_POINTER:
		l1 = get_int(*pbuf, *psize);
		l2 = get_int(buf, size);
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
		}
		break;
	}
	if (*psize < int_size) {
		*psize = int_size;
		free(*pbuf);
		*pbuf = Malloc((unsigned int) int_size);
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
do_in(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  long		l;
  char		*buf = 0;
  long		size;
  p_type	tp;

  error("IN not implemented"); 	/* ??? */
  return 0;
}

static int
do_array(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  long		l;
  char		*buf = 0;
  long		size;
  p_type	tp;

  error("[ not implemented"); 	/* ??? */
  return 0;
}

static int
do_select(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  long		l;
  char		*buf = 0;
  long		size;
  p_type	tp;

  error("SELECT not implemented"); 	/* ??? */
  return 0;
}

static int (*bin_op[])() = {
  0,
  0,
  0,
  do_andor,
  do_andor,
  do_arith,
  do_arith,
  do_arith,
  do_arith,
  do_in,
  do_array,
  do_arith,
  do_arith,
  do_arith,
  do_cmp,
  do_cmp,
  do_cmp,
  do_cmp,
  do_cmp,
  do_cmp,
  do_select
};

int
eval_expr(p, pbuf, psize, ptp)
  p_tree	p;
  char		**pbuf;
  long		*psize;
  p_type	*ptp;
{
  register p_symbol	sym;
  int	retval = 0;

  switch(p->t_oper) {
  case OP_NAME:
  case OP_SELECT:
	sym = identify(p, VAR|REGVAR|LOCVAR|VARPAR|CONST);
	if (! sym) return 0;
	if (! get_value(sym, pbuf, psize)) {
		print_node(p, 0);
		fputs(" currently not available\n", db_out);
		break;
	}
	*ptp = sym->sy_type;
	retval = 1;
	break;

  case OP_INTEGER:
	*pbuf = Malloc(sizeof(long));
	*psize = sizeof(long);
	*ptp = long_type;
	*((long *) (*pbuf)) = p->t_ival;
	retval = 1;
	break;

  case OP_REAL:
	*pbuf = Malloc(sizeof(double));
	*psize = sizeof(double);
	*ptp = double_type;
	*((double *) (*pbuf)) = p->t_fval;
	retval = 1;
	break;

  case OP_STRING:
	*pbuf = Malloc(sizeof(char *));
	*psize = sizeof(char *);
	*ptp = string_type;
	*((char **) (*pbuf)) = p->t_sval;
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
  return retval;
}
