/* $Id$ */

/* Scope mechanism */

#include	<assert.h>
#include	<alloc.h>
#include	<out.h>

#include	"position.h"
#include	"file.h"
#include	"idf.h"
#include	"type.h"
#include	"symbol.h"
#include	"scope.h"
#include	"avl.h"

p_scope PervasiveScope, CurrentScope, FileScope;

/* STATICALLOCDEF "scope" 10 */

static AVL_tree	ScopeTree;

static int
cmp_starts(s1, s2)
  char	*s1, *s2;
{
  register p_scope c1 = (p_scope)s1, c2 = (p_scope)s2;

  return c1->sc_start < c2->sc_start
	 ? -1
	 : c1->sc_start == c2->sc_start
	   ? 0
	   : 1;
}

/*ARGSUSED*/
open_scope(name, has_activation)
  p_symbol name;
  int has_activation;
{
  register p_scope sc = new_scope();

  sc->sc_has_activation_record = has_activation;
  sc->sc_static_encl = CurrentScope;
  sc->sc_definedby = name;
  sc->sc_proclevel = CurrentScope->sc_proclevel;
			/* sc_proclevel possibly reset by caller */
  CurrentScope = sc;
}

init_scope()
{
  register p_scope sc = new_scope();

  PervasiveScope = sc;
  CurrentScope = sc;
  open_scope((p_symbol) 0, 0);		/* this one will be closed at the
					   first N_SO
					*/
  ScopeTree = create_avl_tree(cmp_starts);
}

close_scope()
{
  register p_scope sc = CurrentScope;

  assert(sc != 0);
  CurrentScope = sc->sc_static_encl;
}

add_scope_addr(scope)
  p_scope	scope;
{
  add_to_avl_tree(ScopeTree, (char *)scope);
}

/* extern p_scope	get_scope_from_addr(t_addr a);
   Returns the scope of the code at address 'a', or 0 if it could not be found.
*/
p_scope
get_scope_from_addr(a)
  t_addr a;
{
  t_scope sc;

  sc.sc_start = a;
  return (p_scope) find_ngt(ScopeTree, (char *) &sc);
}

/* extern p_scope	get_next_scope_from_addr(t_addr a);
   Returns the scope following the one of the code at address 'a',
   and that has an activation record,
   or 0 if it could not be found.
*/
p_scope
get_next_scope_from_addr(a)
  t_addr a;
{
  t_scope sc;

  sc.sc_start = a;
  for (;;) {
  	p_scope psc = (p_scope) find_nlt(ScopeTree, (char *) &sc);
	if (! psc || psc->sc_has_activation_record) return psc;
	sc.sc_start = psc->sc_start+1;
  }
  /*NOTREACHED*/
}

/* extern int	has_static_link(p_scope sc);
   Returns 1 if the procedure of this scope takes a static link.
*/
int
has_static_link(sc)
  register p_scope	sc;
{
  return sc->sc_proclevel > 1;
}

/* extern p_scope	base_scope(p_scope sc);
   Returns the closest enclosing scope of 'sc' that has an activation record.
*/
p_scope
base_scope(sc)
  register p_scope	sc;
{
  while (sc && ! sc->sc_has_activation_record) {
	sc = sc->sc_static_encl;
  }
  return sc;
}

/* extern int	scope_encloses(p_scope scope, from_scope);
   Returns 1 if scope encloses from from_scope, 0 otherwise.
*/
int
scope_encloses(scope, from_scope)
  p_scope	scope, from_scope;
{
  register p_scope sc = from_scope;

  while (sc) {
	if (sc == scope) return 1;
	sc = sc->sc_static_encl;
  }
  return 0;
}
