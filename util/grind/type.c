/* $Id$ */

/* Routines to create type structures */

#include <alloc.h>
#include <assert.h>

#include "idf.h"
#include "type.h"
#include "symbol.h"
#include "scope.h"
#include "langdep.h"
#include "expr.h"
#include "misc.h"

p_type	int_type, char_type, short_type, long_type, bool_type;
p_type	uint_type, uchar_type, ushort_type, ulong_type;
p_type	void_type;
p_type	float_type, double_type;
p_type	string_type, address_type;

long	int_size = sizeof(int),
	char_size = 1,
	short_size = sizeof(short),
	long_size = sizeof(long),
	pointer_size = sizeof(char *);

long	float_size = sizeof(float),
	double_size = sizeof(double);

struct bounds {
	long low, high;
};

static struct bounds ibounds[2] = {
	{ -128, 127 },
	{ -32768, 32767 }
};

static struct bounds ubounds[2] = {
	{ 0, 255 },
	{ 0, 65535 }
};

static long max_int[8], max_uns[8];

struct integer_types {
	long	maxval;
	p_type	type;
};

static struct integer_types i_types[4];
static struct integer_types u_types[4];

#define ufit(n, nb)	Xfit(n, nb, ubounds)
#define ifit(n, nb)	Xfit(n, nb, ibounds)
#define Xfit(n, nb, b)	((n) >= (b)[(nb)-1].low && (n) <= (b)[(nb)-1].high)

/* Create a subrange type, but is it really a subrange? */
p_type
subrange_type(A, base_index, c1, c2, result_index)
  int *base_index, *result_index;
  long c1, c2;
{
  int itself = 0;
  register p_type p;
  p_type base_type;

  if (!A) {
	/* Subrange of itself is a special case ... */
	if (result_index &&
 	   result_index[0] == base_index[0] &&
 	   result_index[1] == base_index[1]) {

		/* c1 = 0 and c2 = 0 -> void */
		if (c1 == 0 && c2 == 0) {
			return void_type;
		}

		if ((c1 == 0 || c1 == -128) && c2 == 127) {
			return char_type;
		}

		if (c1 == 0 && c2 == 255) {
			return uchar_type;
		}

		itself = 1;
	}
  }

  if (itself) base_type = int_type; else base_type = *(tp_lookup(base_index));

  if (! A) {
	/* c2 = 0 and c1 > 0 -> real */
	if (c2 == 0 && c1 > 0) {
		if (c1 == float_size) return float_type;
		return double_type;
	}

	/* c1 = 0 and base_index indicates int_type or itself -> unsigned,
	   c1 = -c2 - 1 and base_index indicates int_type or itself -> integer
	*/
	if (itself || base_type == int_type) {
		register struct integer_types *ip = 0;
		if (c1 == 0) {
			ip = &u_types[0];
		}
		else if (c1 == -c2 - 1) {
			ip = &i_types[0];
		}
		if (ip) {
			while (ip->maxval != 0 && ip->maxval != c2) ip++;
			if (ip->maxval) return ip->type;
		}
	}
  }
  /* if we get here, it actually is a subrange type */
  p = new_type();
  p->ty_class = T_SUBRANGE;
  p->ty_low = c1;
  p->ty_up = c2;
  p->ty_base = base_type;
  p->ty_A = A;

  /* determine size of subrange type */
  p->ty_size = base_type->ty_size;
  if (!A && p->ty_base == uint_type) {
  	if (ufit(p->ty_up, 1)) {
		p->ty_size = 1;
  	}
  	else if (ufit(p->ty_up, (int)short_size)) {
		p->ty_size = short_size;
	}
  }
  if (!A && p->ty_base == int_type) {
  	if (ifit(p->ty_up, 1) && ifit(p->ty_low, 1)) {
		p->ty_size = 1;
  	}
  	else if (ifit(p->ty_up, (int)short_size) &&
		 ifit(p->ty_low, (int)short_size)) {
		p->ty_size = short_size;
	}
  }

  return p;
}

static long
nel(tp)
  register p_type tp;
{
  switch(tp->ty_class) {
  case T_SUBRANGE:
	if (tp->ty_A) return 0;
	if (tp->ty_low <= tp->ty_up) return tp->ty_up - tp->ty_low + 1;
	return tp->ty_low - tp->ty_up + 1;
  case T_UNSIGNED:
  case T_INTEGER:
	if (tp->ty_size == 1) return 256;
	if (tp->ty_size == 2) return 65536L;
	assert(0);
	break;
  case T_ENUM:
	return tp->ty_nenums;
  default:
	assert(0);
	break;
  }
  return 0;
}

p_type
array_type(bound_type, el_type)
  p_type bound_type, el_type;
{
  register p_type tp = new_type();

  tp->ty_class = T_ARRAY;
  tp->ty_index = bound_type;
  switch(bound_type->ty_class) {
  case T_SUBRANGE:
	if (bound_type->ty_A) break;
	tp->ty_lb = bound_type->ty_low;
	tp->ty_hb = bound_type->ty_up;
	break;
  case T_ENUM:
	tp->ty_lb = 0;
	tp->ty_hb = bound_type->ty_nenums-1;
	break;
  case T_UNSIGNED:
	tp->ty_lb = 0;
	tp->ty_hb = bound_type->ty_size == 1 ? 255 : 65535L;
	break;
  case T_INTEGER:
	tp->ty_lb = bound_type->ty_size == 1 ? -128 : -32768;
	tp->ty_hb = bound_type->ty_size == 1 ? 127 : 32767;
	break;
  }
  tp->ty_elements = el_type;
  tp->ty_size = (*currlang->arrayelsize)(el_type->ty_size) * nel(bound_type);
  return tp;
}

p_type
basic_type(fund, size)
  int	fund;
  long	size;
{
  register p_type	p = new_type();

  p->ty_class = fund;
  p->ty_size = size;
  return p;
}

set_bounds(tp)
  register p_type	tp;
{
  /* Determine the size and low of a set type */
  register p_type base = tp->ty_setbase;

  if (base->ty_class == T_SUBRANGE) {
	tp->ty_size = (base->ty_up - base->ty_low + 7) >> 3;
	tp->ty_setlow = base->ty_low;
  }
  else if (base->ty_class == T_INTEGER) {
	tp->ty_size = (max_int[(int)base->ty_size] + 1) >>  2;
	tp->ty_setlow = -max_int[(int)base->ty_size] - 1;
  }
  else {
	assert(base->ty_class == T_UNSIGNED);
	tp->ty_size = (max_uns[(int)base->ty_size] + 1) >>  3;
	tp->ty_setlow = 0;
  }
}

init_types()
{
  register int i = 0;
  register long x = 0;

  while (x >= 0) {
	i++;
	x = (x << 8) + 0377;
	max_uns[i] = x;
	max_int[i] = x & ~(1L << (8*i - 1));
  }
  int_type = basic_type(T_INTEGER, int_size);
  long_type = basic_type(T_INTEGER, long_size);
  short_type = basic_type(T_INTEGER, short_size);
  char_type = basic_type(T_INTEGER, char_size);
  uint_type = basic_type(T_UNSIGNED, int_size);
  ulong_type = basic_type(T_UNSIGNED, long_size);
  ushort_type = basic_type(T_UNSIGNED, short_size);
  uchar_type = basic_type(T_UNSIGNED, char_size);
  string_type = basic_type(T_STRING, 0L);
  address_type = basic_type(T_POINTER, pointer_size);
  void_type = basic_type(T_VOID, 0L);
  float_type = basic_type(T_REAL, float_size);
  double_type = basic_type(T_REAL, double_size);

  i_types[0].maxval = max_int[(int)int_size]; i_types[0].type = int_type;
  i_types[1].maxval = max_int[(int)short_size]; i_types[1].type = short_type;
  i_types[2].maxval = max_int[(int)long_size]; i_types[2].type = long_type;
  u_types[0].maxval = max_uns[(int)int_size]; u_types[0].type = uint_type;
  u_types[1].maxval = max_uns[(int)short_size]; u_types[1].type = ushort_type;
  u_types[2].maxval = max_uns[(int)long_size]; u_types[2].type = ulong_type;
}

/*
 * Some code to handle type indices, which are pairs of integers.
 * What we need is a two-dimensional array, but we don't know how large
 * it is going to be, so we use a list of rows instead.
 */
static struct tp_index {
  unsigned	len;
  p_type	**row;
} *list_row;
static unsigned list_len;

#define NINCR 10
  
p_type *
tp_lookup(type_index)
  int *type_index;
{
  register int i;
  register struct tp_index *p;

  while (type_index[0] >= list_len) {
	if (list_len) {
		list_row = (struct tp_index *) Realloc((char *) list_row,
				(list_len += NINCR) * sizeof(struct tp_index));
	}
	else	list_row = (struct tp_index *)
			Malloc((list_len = NINCR) * sizeof(struct tp_index));
	for (i = NINCR; i > 0; i--) {
		list_row[list_len - i].len = 0;
	}
  }
  p = &list_row[type_index[0]];
  while (type_index[1] >= p->len) {
	int indx = p->len/NINCR;
	if (p->len) {
		p->row = (p_type **) Realloc((char *) p->row,
				(unsigned) (indx + 1) * sizeof(p_type *));
	}
	else	p->row = (p_type **) Malloc(sizeof(p_type *));
	p->len += NINCR;
	p->row[indx] = (p_type *) Malloc(NINCR * sizeof(p_type));
	for (i = NINCR-1; i >= 0; i--) {
		p->row[indx][i] = 0;
	}
  }
  return &(p->row[type_index[1]/NINCR][type_index[1]%NINCR]);
}

clean_tp_tab()
{
  if (list_len) {
  	register int i = list_len;

  	while (--i >= 0) {
		register int j = list_row[i].len;
		if (j) {
			while (--j > 0) {
				p_type p = list_row[i].row[j/NINCR][j%NINCR];
				if (p && p->ty_class == 0) {
					error("%s: incomplete type (%d,%d)",
					      FileScope->sc_definedby->sy_idf->id_text,
					      i,
					      j);
				}
			}
			j = list_row[i].len;
			while (j > 0) {
				free((char *) list_row[i].row[j/NINCR-1]);
				j -= NINCR;
			}
			free((char *) list_row[i].row);
		}
	}
	free((char *) list_row);
	list_len = 0;
	list_row = 0;
  }
}

end_literal(tp, maxval)
  register p_type tp;
  long maxval;
{
  tp->ty_literals = (struct literal *)
	Realloc((char *) tp->ty_literals,
		tp->ty_nenums * sizeof(struct literal));
  if (ufit(maxval, 1)) tp->ty_size = 1;
  else if (ufit(maxval, (int)short_size)) tp->ty_size = short_size;
  else tp->ty_size = int_size;
  if (! bool_type) bool_type = tp;
}

long
param_size(t, v)
  int	v;
  p_type t;
{
  if (v == 'i' || v == 'v') {
	/* addresss; only exception is a conformant array, which also
	   takes a descriptor.
	*/
	if (currlang == m2_dep &&
	    t->ty_class == T_ARRAY &&
	    t->ty_index->ty_class == T_SUBRANGE &&
	    t->ty_index->ty_A) {
		return pointer_size + 3 * int_size;
	}
	return pointer_size;
  }
  return ((t->ty_size + int_size - 1) / int_size) * int_size;
}

add_param_type(v, s)
  int	v;		/* 'v' or 'i' for address, 'p' for value */
  p_symbol s;		/* parameter itself */
{
  register p_scope sc = base_scope(s->sy_scope);
  register p_type prc_type;

  if (! sc) return;
  prc_type = sc->sc_definedby->sy_type;
  assert(prc_type->ty_class == T_PROCEDURE);

  if (v == 'Z') {
  	prc_type->ty_nbparams += 3 * int_size;
	return;
  }
  prc_type->ty_nparams++;
  prc_type->ty_params = (struct param *) Realloc((char *) prc_type->ty_params, 
				(unsigned)prc_type->ty_nparams * sizeof(struct param));
  prc_type->ty_params[prc_type->ty_nparams - 1].par_type = s->sy_type;
  prc_type->ty_params[prc_type->ty_nparams - 1].par_kind = v;
  prc_type->ty_params[prc_type->ty_nparams - 1].par_off = s->sy_name.nm_value;
  prc_type->ty_nbparams += param_size(s->sy_type, v);
}

/* Compute the size of a parameter of dynamic size
*/

long
compute_size(tp, AB)
  p_type	tp;
  char		*AB;
{
  long	low, high;

  assert(tp->ty_class == T_ARRAY);
  assert(tp->ty_index->ty_class == T_SUBRANGE);
  assert(tp->ty_index->ty_A != 0);

  if (tp->ty_index->ty_A & 1) {
	low = get_int(AB+tp->ty_index->ty_low, int_size, T_INTEGER);
  } else low = tp->ty_index->ty_low;
  tp->ty_lb = low;
  if (tp->ty_index->ty_A & 2) {
	high = get_int(AB+tp->ty_index->ty_up, int_size, T_INTEGER);
  } else if (tp->ty_index->ty_A & 0200) {
	high = get_int(AB+tp->ty_index->ty_up, int_size, T_INTEGER);
	high += get_int(AB+tp->ty_index->ty_up+int_size, int_size, T_INTEGER);
  } else high = tp->ty_index->ty_up;
  tp->ty_hb = high;
  return (high - low + 1) * tp->ty_elements->ty_size;
}
