/* $Header$ */

#include	<stdio.h>
#include	<assert.h>
#include	<alloc.h>
#include	<out.h>
#include	<stb.h>

#include	"position.h"
#include	"scope.h"
#include	"file.h"
#include	"idf.h"
#include	"symbol.h"
#include	"misc.h"

extern FILE	*db_out;

static struct outname *f_start, *f_end;

/* extern p_position get_position_from_addr(t_addr t);
   Returns a pointer to a structure containing the source position of the code
   at address 't'.  0 is returned if no source position could be found.
*/
p_position
get_position_from_addr(t)
  t_addr t;
{
  static t_position retval;
  register struct outname *p;
  register int i,j;

  if (! f_start) return 0;
  i = 0;
  j = f_end - f_start;
  do {
	p = &f_start[((i + j) >> 1) + ((i + j) & 1)];
	while ((p->on_type >> 8) != N_SLINE) p++;
	if (p->on_valu > t) {
		p--;
		while (p > &f_start[i] && (p->on_type >> 8) != N_SLINE) p--;
		j = p-f_start;
	}
	else	i = p-f_start;
  } while (i < j);
  retval.lineno = f_start[j].on_desc;
  p = &f_start[j-1];
  while ((i = p->on_type >> 8) != N_SOL && i != N_SO) p--;
  retval.filename = p->on_mptr;
  return &retval;
}

/* extern t_addr get_addr_from_position(p_position p);
   Returns the address of the code at position 'p', or ILL_ADDR if it could
   not be found. If there is no symbolic information for the filename in
   position 'p', an error message will be given.
*/
t_addr
get_addr_from_position(p)
  p_position p;
{
  register p_symbol sym = Lookup(findidf(p->filename), PervasiveScope, FILESYM);

  if (sym) {
	register unsigned int i;
	register p_file map = sym->sy_file;

	for (i = p->lineno; i > 0; i--) {
		register struct outname *n = map->f_line_addr[HASH(i)];

		while (n) {
			if (n->on_desc == i) return (t_addr) n->on_valu;
			n = next_outname(n);
		}
	}
	return ILL_ADDR;
  }
  error("no symbolic information for file %s", p->filename);
  return ILL_ADDR;
}

/* extern add_position_addr(char *filename, struct outname *n);
   Adds the ('filename','lineno'),'t' pair to the mapping information.
*/
add_position_addr(filename, n)
  char *filename;
  register struct outname *n;
{
  static char *lastfile = 0;
  static p_file lastmap = 0;
  register p_file map = lastmap;

  if (filename != lastfile) {	/* new file ... */
	register p_symbol sym;

	lastfile = filename;
	if (! filename) {	/* last call */
		return;
	}
	sym = Lookup(findidf(filename), PervasiveScope, FILESYM);
	if (sym) map = sym->sy_file; 
	else {
		sym = add_file(filename);
		map = sym->sy_file;
		map->f_scope = FileScope;
	}
	lastmap = map;
	if (! f_start) f_start = n;
  }
  else map = lastmap;
  if (map) {
  	f_end = n;
  	setnext_outname(n, map->f_line_addr[HASH(n->on_desc)]);
  	map->f_line_addr[HASH(n->on_desc)] = n;
  }
}

/* extern p_position print_position(t_addr a, int print_function);
   Prints position 'a' and returns it. If 'print_function' is set,
   an attempt is made to print the function name as well.
*/
p_position
print_position(a, print_function)
  t_addr	a;
  int		print_function;
{
  register p_position	pos = get_position_from_addr(a);

  if (print_function) {
  	register p_scope sc = base_scope(get_scope_from_addr(a));
	if (sc) fprintf(db_out, "in %s ", sc->sc_definedby->sy_idf->id_text);
  }
  if (pos) fprintf(db_out, "at \"%s\":%u", pos->filename, pos->lineno);
  return pos;
}
