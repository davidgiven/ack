/* $Header$ */

#include <alloc.h>
#include <assert.h>
#include <stdio.h>

#include "type.h"
#include "message.h"
#include "langdep.h"
#include "scope.h"
#include "symbol.h"
#include "position.h"
#include "idf.h"

extern FILE *db_out;
extern long float_size, pointer_size, int_size;

static
print_literal(tp, v)
  p_type	tp;
  int		v;
{
  register struct literal *lit = tp->ty_literals;
  register int i;

  for (i = tp->ty_nenums; i; i--, lit++) {
	if (lit->lit_val == v) {
		fprintf(db_out, lit->lit_name);
		break;
	}
  }
  if (! i) {
	fprintf(db_out, "unknown enumeration value %d", v);
  }
}

static
print_unsigned(tp, v)
  p_type	tp;
  long		v;
{
  if (tp == uchar_type) {
	fprintf(db_out, currlang->char_fmt, (int) v);
  }
  else	fprintf(db_out, currlang->uns_fmt, v);
}

static
print_integer(tp, v)
  p_type	tp;
  long		v;
{
  if (tp == char_type) {
	fprintf(db_out, currlang->char_fmt, (int) v);
  }
  else	fprintf(db_out, currlang->decint_fmt, v);
}

print_params(tp, AB, static_link)
  p_type	tp;
  t_addr	AB;
{
  char *param_bytes;
  register char *p;
  register int i;
  register struct param *par;
  long size;

  if (! tp) return;
  assert(tp->ty_class == T_PROCEDURE);

  if ((i = tp->ty_nparams) == 0) return;

  /* get parameter bytes */
  par = tp->ty_params;
  size = tp->ty_nbparams;
  if (static_link) size += pointer_size;
  param_bytes = p = Malloc((unsigned)size);
  if (static_link) p += pointer_size;
  if (! get_bytes(size, AB, param_bytes)) {
	error("no debuggee");
	free(param_bytes);
	return;
  }

  while (i--) {
	if (par->par_kind == 'v' || par->par_kind == 'i') {
		/* call by reference parameter, or
		   call by value parameter, but address is passed;
		   try and get value.
		*/
		char	*q;

		if ((size = par->par_type->ty_size) == 0) {
			size = compute_size(par->par_type, param_bytes);
		}
		q = Malloc((unsigned) size);
		if (! get_bytes(size, (t_addr) BUFTOA(p), q)) {
			fprintf(db_out, currlang->addr_fmt, BUFTOA(p));
		}
		else {
			print_val(par->par_type, size, q, 1, 0);
		}
		free(q);
	}
	else print_val(par->par_type, par->par_type->ty_size, p, 1, 0);
	if (i) fputs(", ", db_out);
	p += param_size(par->par_type, par->par_kind);
	par++;
  }
  free(param_bytes);
}

print_val(tp, tp_sz, addr, compressed, indent)
  p_type	tp;		/* type of value to be printed */
  long		tp_sz;		/* size of object to be printed */
  char		*addr;		/* address to get value from */
  int		compressed;	/* for parameter lists */
  int		indent;		/* indentation */
{
  register int i;
  long elsize;

  if (indent == 0) indent = 4;
  switch(tp->ty_class) {
  case T_SUBRANGE:
	print_val(tp->ty_base, tp->ty_size, addr, compressed, indent);
	break;
  case T_ARRAY:
	if (tp->ty_elements == char_type ||
	    tp->ty_elements == uchar_type) {
		print_val(string_type, tp_sz, addr, compressed, indent);
		break;
	}
	if (compressed) {
		fprintf(db_out, currlang->open_array_display);
	}
	else {
		fprintf(db_out, "\n%*c%s%*c",
			indent,
			' ',
			currlang->open_array_display,
			4-strlen(currlang->open_array_display), ' ');
	}
	indent += 4;
	elsize = (*currlang->arrayelsize)(tp->ty_elements->ty_size);
	for (i = tp_sz/elsize; i; i--) {
		print_val(tp->ty_elements, tp->ty_elements->ty_size, addr, compressed, indent);
		addr += elsize;
		if (compressed && i > 1) {
			fprintf(db_out, ", ...");
			break;
		} 
		if (i > 1) {
			fputc(',', db_out);
		}
		fprintf(db_out, "\n%*c", i > 1 ? indent : indent - 4, ' ');
	}
	fprintf(db_out, currlang->close_array_display);
	indent -= 4;
	break;
  case T_STRUCT: {
	register struct fields *fld = tp->ty_fields;

	if (compressed) {
		fprintf(db_out, currlang->open_struct_display);
	}
	else {
		fprintf(db_out, "\n%*c%s%*c",
			indent,
			' ',
			currlang->open_struct_display,
			4-strlen(currlang->open_struct_display), ' ');
	}
	indent += 4;
	for (i = tp->ty_nfields; i; i--, fld++) {
		long sz = fld->fld_type->ty_size;
		if (! compressed) fprintf(db_out, "%s = ", fld->fld_name);
		if (fld->fld_bitsize < sz << 3) {
			/* apparently a bit field */
			/* ??? */
			fprintf(db_out, "<bitfield, %d, %ld>", fld->fld_bitsize, sz);
		}
		else print_val(fld->fld_type, sz, addr+(fld->fld_pos>>3), compressed, indent);
		if (compressed && i > 1) {
			fprintf(db_out, ", ...");
			break;
		} 
		if (i > 1) {
			fputc(',', db_out);
		}
		fprintf(db_out, "\n%*c", i > 1 ? indent : indent - 4, ' ');
	}
	indent -= 4;
	fprintf(db_out, currlang->close_struct_display);
	break;
	}
  case T_UNION:
	fprintf(db_out, "<union>");
	break;
  case T_ENUM:
	print_literal(tp,  tp_sz == 1 
			   ? (*addr & 0xFF)
			   : tp_sz == 2
			      ? (BUFTOS(addr) & 0xFFFF)
			      : (int) BUFTOL(addr));
	break;
  case T_PROCEDURE: {
	register p_scope sc = get_scope_from_addr((t_addr) BUFTOA(addr));

	if (sc && sc->sc_definedby) {
		fprintf(db_out, sc->sc_definedby->sy_idf->id_text);
		break;
	}
	}
	/* Fall through */
  case T_POINTER:
	fprintf(db_out, currlang->addr_fmt, (long) BUFTOA(addr));
	break;
  case T_FILE:
	fprintf(db_out, "<file>");
	break;
  case T_SET: {
	long	val = tp->ty_setlow;
	p_type	base = tp->ty_setbase;
	long	nelements = tp->ty_size << 3;
	int	count = 0;
	int	rsft = 3 + (int_size == 2 ? 1 : 2);
	long	mask = int_size == 2 ? 0xFFFF : 0xFFFFFFFF;

	if (base->ty_class == T_SUBRANGE) base = base->ty_base;
	if (compressed) {
		fprintf(db_out, currlang->open_set_display);
	}
	else {
		fprintf(db_out, "\n%*c%s%*c",
			indent,
			' ',
			currlang->open_set_display,
			4-strlen(currlang->open_set_display), ' ');
	}
	indent += 4;
	for (i = 0; i < nelements; i++) {
		if (*((int *) addr + (i >> rsft)) & (1 << (i & mask))) {
			count++;
			if (count > 1) {
				if (compressed) {
					fprintf(db_out, ", ...");
					break;
				}
				fprintf(db_out, ",\n%*c", indent , ' ');
			}
			switch(base->ty_class) {
			case T_INTEGER:
				print_integer(base, val+i);
				break;
			case T_UNSIGNED:
				print_unsigned(base, val+i);
				break;
			case T_ENUM:
				print_literal(base, (int)val+i);
				break;
			default:
				assert(0);
			}
		} 
	}
	if (! compressed) {
		fprintf(db_out, "\n%*c", indent-4 , ' ');
	}
	indent -= 4;
	fprintf(db_out, currlang->close_set_display);
  	}
	break;
  case T_REAL: {
	double val = tp->ty_size == float_size
		? BUFTOF(addr)
		: BUFTOD(addr);
	fprintf(db_out, currlang->real_fmt, val);
	break;
	}
  case T_UNSIGNED:
	print_unsigned(tp, tp_sz == 1 
				? (*addr & 0xFF)
				: tp_sz == 2
			  	    ? (BUFTOS(addr) & 0xFFFF)
			  	    : BUFTOL(addr));
	break;
  case T_INTEGER:
	print_integer(tp, tp_sz == 1 
				? *addr
				: tp_sz == 2
			  	    ? BUFTOS(addr)
			  	    : BUFTOL(addr));
	break;
  case T_STRING:
	(*currlang->printstring)(addr, (int) tp_sz);
	break;
  default:
	assert(0);
	break;
  }
}

int
print_sym(sym)
  p_symbol	sym;
{
  char		*buf;
  long		size;

  if (get_value(sym, &buf, &size)) {
	fputs(" = ", db_out);
	print_val(sym->sy_type, size, buf, 0, 0);
	if (buf) free(buf);
	fputs("\n", db_out);
	return 1;
  }
  return 0;
}
