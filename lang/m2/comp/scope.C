/* S C O P E   M E C H A N I S M */

static char *RcsId = "$Header$";

#include	<assert.h>
#include	<alloc.h>
#include	"scope.h"

static int maxscope;		/* maximum assigned scope number */

struct scope *CurrentScope;

/* STATICALLOCDEF "scope" */

/*	Open a scope that is either open (automatic imports) or closed.
	A closed scope is handled by adding an extra entry to the list
	with scope number 0. This has two purposes: it makes scope 0
	visible, and it marks the end of a visibility list.
	Scope 0 is the pervasive scope, the one that is always visible.
	A disadvantage of this method is that we cannot open scope 0
	explicitly.
*/
open_scope(scopetype, scopenr)
{
	register struct scope *sc = new_scope();
	register struct scope *sc1;

	sc->sc_scope = scopenr == 0 ? ++maxscope : scopenr;
	assert(scopetype == OPENSCOPE || scopetype == CLOSEDSCOPE);
	sc1 = CurrentScope;
	if (scopetype == CLOSEDSCOPE) {
		sc1 = new_scope();
		sc1->sc_scope = 0;			/* Pervasive scope nr */
		sc1->next = CurrentScope;
	}
	sc->next = sc1;
	CurrentScope = sc;
}

close_scope()
{
	register struct scope *sc = CurrentScope;

	assert(sc != 0);
	if (sc->next && (sc->next->sc_scope == 0)) {
		struct scope *sc1 = sc;

		sc = sc->next;
		free_scope(sc1);
	}
	CurrentScope = sc->next;
	free_scope(sc);
}

init_scope()
{
	register struct scope *sc = new_scope();

	sc->sc_scope = 0;
	sc->next = 0;
	CurrentScope = sc;
}
