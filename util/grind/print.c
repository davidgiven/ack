/* $Header$ */

#include <alloc.h>
#include <assert.h>
#include <stdio.h>

#include "type.h"
#include "langdep.h"
#include "scope.h"
#include "symbol.h"
#include "position.h"
#include "idf.h"
#include "expr.h"
#include "misc.h"

extern FILE *db_out;
extern char *strindex();
extern char *malloc();

static
print_unsigned(tp, v, format)
  p_type	tp;
  long		v;
  register char	*format;
{
  while (format && *format) {
	if (strindex("cdohx", *format)) break;
	format++;
  }
  switch(format == 0 ? 0 : *format) {
  case 'u':
  	fprintf(db_out, currlang->uns_fmt, v);
	break;
  default:
	if (tp != uchar_type) {
  		fprintf(db_out, currlang->uns_fmt, v);
		break;
	}
	/* fall through */
  case 'c':
	(*currlang->printchar)((int) v);
	break;
  case 'd':
  	fprintf(db_out, currlang->decint_fmt, v);
	break;
  case 'o':
  	fprintf(db_out, currlang->octint_fmt, v);
	break;
  case 'x':
  case 'h':
  	fprintf(db_out, currlang->hexint_fmt, v);
	break;
  }
}

static
print_literal(tp, v, compressed, format)
  p_type	tp;
  long		v;
  int		compressed;
  char		*format;
{
  register struct literal *lit = tp->ty_literals;
  register int i;

  if (format) {
	print_unsigned(tp, v, format);
	return;
  }
  for (i = tp->ty_nenums; i; i--, lit++) {
	if (lit->lit_val == v) {
		fputs(lit->lit_name, db_out);
		break;
	}
  }
  if (! i) {
	fprintf(db_out,
		compressed ? "?%ld?" : "unknown enumeration value %ld",
		v);
  }
}

static
print_integer(tp, v, format)
  p_type	tp;
  long		v;
  register char	*format;
{
  while (format && *format) {
	if (strindex("cdohx", *format)) break;
	format++;
  }
  switch(format == 0 ? 0 : *format) {
  default:
	if (tp != char_type) {
  		fprintf(db_out, currlang->decint_fmt, v);
		break;
	}
	/* fall through */
  case 'c':
	(*currlang->printchar)((int) v);
	break;
  case 'd':
  	fprintf(db_out, currlang->decint_fmt, v);
	break;
  case 'o':
  	fprintf(db_out, currlang->octint_fmt, v);
	break;
  case 'x':
  case 'h':
  	fprintf(db_out, currlang->hexint_fmt, v);
	break;
  case 'u':
  	fprintf(db_out, currlang->uns_fmt, v);
	break;
  }
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
  param_bytes = p = malloc((unsigned)size);
  if (! p) {
	error("could not allocate enough memory");
	return;
  }
  if (! get_bytes(size, AB, p)) {
	free(p);
	return;
  }

  while (i--) {
	p = param_bytes + par->par_off;
	if (par->par_kind == 'v' || par->par_kind == 'i') {
		/* call by reference parameter, or
		   call by value parameter, but address is passed;
		   try and get value.
		*/
		char	*q;
		t_addr	addr = get_int(p, pointer_size, T_UNSIGNED);

		if ((size = par->par_type->ty_size) == 0) {
			size = compute_size(par->par_type, param_bytes);
		}
		q = malloc((unsigned) size);
  		if (! q) {
			error("could not allocate enough memory");
			free(param_bytes);
			return;
  		}
		if (! get_bytes(size, addr, q)) {
			fprintf(db_out, currlang->addr_fmt, (long) addr);
		}
		else {
			print_val(par->par_type, size, q, 1, 0, (char *)0);
		}
		free(q);
	}
	else print_val(par->par_type, par->par_type->ty_size, p, 1, 0, (char *)0);
	if (i) fputs(", ", db_out);
	par++;
  }
  free(param_bytes);
}

print_val(tp, tp_sz, addr, compressed, indent, format)
  p_type	tp;		/* type of value to be printed */
  long		tp_sz;		/* size of object to be printed */
  char		*addr;		/* address to get value from */
  int		compressed;	/* for parameter lists */
  int		indent;		/* indentation */
  register char	*format;	/* format given or 0 */
{
  register int i;
  long elsize;

  if (indent == 0) indent = 4;
  switch(tp->ty_class) {
  case T_CROSS:
	if (! tp->ty_cross) {
		error("unknown type");
		break;
	}
	print_val(tp->ty_cross, tp_sz, addr, compressed, indent, format);
	break;
  case T_SUBRANGE:
	print_val(tp->ty_base, tp_sz, addr, compressed, indent, format);
	break;
  case T_ARRAY:
	if ((!format || strindex(format, 'a') == 0) &&
	    (tp->ty_elements == char_type ||
	     tp->ty_elements == uchar_type)) {
		print_val(string_type, tp_sz, addr, compressed, indent, format);
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
		print_val(tp->ty_elements, tp->ty_elements->ty_size, addr, compressed, indent, format);
		addr += elsize;
		if (compressed && i > 1) {
			fprintf(db_out, ", ...");
			break;
		} 
		if (i > 1) {
			putc(',', db_out);
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
		if (fld->fld_bitsize < (sz << 3)) {
			/* apparently a bit field */
			/* ??? */
			fprintf(db_out, "<bitfield, %ld, %ld>", fld->fld_bitsize, sz);
		}
		else print_val(fld->fld_type, sz, addr+(fld->fld_pos>>3), compressed, indent, format);
		if (compressed && i > 1) {
			fprintf(db_out, ", ...");
			break;
		} 
		if (i > 1) {
			putc(',', db_out);
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
	print_literal(tp, get_int(addr, tp_sz, T_ENUM), compressed, format);
	break;
  case T_PROCEDURE: {
	register p_scope sc = get_scope_from_addr((t_addr) get_int(addr, pointer_size, T_UNSIGNED));

	if (sc && sc->sc_definedby) {
		fprintf(db_out, sc->sc_definedby->sy_idf->id_text);
		break;
	}
	}
	fprintf(db_out, currlang->addr_fmt, get_int(addr, pointer_size, T_UNSIGNED));
	break;
  case T_POINTER: {
	t_addr a = get_int(addr, tp_sz, T_UNSIGNED);

	fprintf(db_out, currlang->addr_fmt, a);
	if (format && strindex(format, 's') &&
	    (tp->ty_ptrto == char_type || tp->ty_ptrto == uchar_type)) {
		char *naddr = malloc(512);

		if (! naddr) {
			fputs(" (could not allocate memory)", db_out);
			break;
		}
		if (! get_string(511L, a, naddr)) {
			fputs(" (not a valid pointer)", db_out);
			free(naddr);
			break;
		}
		fputs(" (", db_out);
		print_val(string_type, 512L, naddr, 0, indent, format);
		fputs(")", db_out);
		free(naddr);
		break;
	}
	if (tp->ty_ptrto->ty_class == T_PROCEDURE) {
		p_scope sc = get_scope_from_addr(a);
		if (sc && sc->sc_definedby && a == sc->sc_start) {
			fprintf(db_out, " (%s)", sc->sc_definedby->sy_idf->id_text);
		}
	}
	break;
	}
  case T_FILE:
	fprintf(db_out, "<file>");
	break;
  case T_SET: {
	long	val = tp->ty_setlow;
	p_type	base = tp->ty_setbase;
	long	nelements = tp->ty_size << 3;
	int	count = 0;
	int	rsft = 3 + (int_size == 2 ? 1 : 2);
	long	mask = int_size == 2 ? 017: 037;

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
		if (get_int(addr + (i >> rsft), int_size, T_UNSIGNED) & (1 << (i & mask))) {
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
				print_integer(base, val+i, format);
				break;
			case T_UNSIGNED:
				print_unsigned(base, val+i, format);
				break;
			case T_ENUM:
				print_literal(base, val+i, compressed, format);
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
  case T_REAL:
	fprintf(db_out, currlang->real_fmt, get_real(addr, tp->ty_size));
	break;
  case T_UNSIGNED:
	print_unsigned(tp, get_int(addr, tp_sz, T_UNSIGNED), format);
	break;
  case T_INTEGER:
	print_integer(tp, get_int(addr, tp_sz, T_INTEGER), format);
	break;
  case T_STRING:
	(*currlang->printstring)(db_out, addr, (int) tp_sz);
	break;
  default:
	assert(0);
	break;
  }
}
