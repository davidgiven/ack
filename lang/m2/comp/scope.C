/* S C O P E   M E C H A N I S M */

static char *RcsId = "$Header$";

#include	<assert.h>
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	"LLlex.h"
#include	"idf.h"
#include	"scope.h"
#include	"type.h"
#include	"def.h"
#include	"debug.h"

static int maxscope;		/* maximum assigned scope number */

struct scope *currscope;

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
	DO_DEBUG(debug(1, "Opening a %s scope", scopetype == OPENSCOPE ? "open" : "closed"));
	sc1 = currscope;
	if (scopetype == CLOSEDSCOPE) {
		sc1 = new_scope();
		sc1->sc_scope = 0;		/* Pervasive scope nr */
		sc1->next = currscope;
	}
	sc->next = sc1;
	currscope = sc;
}

static rem_forwards();

close_scope()
{
	register struct scope *sc = currscope;

	assert(sc != 0);
	DO_DEBUG(debug(1, "Closing a scope"));
	if (sc->sc_forw) rem_forwards(sc->sc_forw);
	if (sc->next && (sc->next->sc_scope == 0)) {
		struct scope *sc1 = sc;

		sc = sc->next;
		free_scope(sc1);
	}
	currscope = sc->next;
	free_scope(sc);
}

init_scope()
{
	register struct scope *sc = new_scope();

	sc->sc_scope = 0;
	sc->next = 0;
	currscope = sc;
}

int
uniq_scope()
{
	return ++maxscope;
}

struct forwards {
	struct forwards *next;
	struct token fo_tok;
	struct type **fo_ptyp;
};

/* STATICALLOCDEF "forwards" */

/*	Enter a forward reference into a list belonging to the
	current scope. This is used for POINTER declarations, which
	may have forward references that must howewer be declared in the
	same scope.
*/
Forward(tk, ptp)
	struct token *tk;
	struct type **ptp;
{
	register struct forwards *f = new_forwards();

	f->fo_tok = *tk;
	f->fo_ptyp = ptp;
	f->next = currscope->sc_forw;
	currscope->sc_forw = f;
}

/*	When closing a scope, all forward references must be resolved
*/
static
rem_forwards(fo)
	struct forwards *fo;
{
	register struct forwards *f;
	struct token savetok;
	register struct def *df;
	struct def *lookfor();

	savetok = dot;
	while (f = fo) {
		dot = f->fo_tok;
		df = lookfor(dot.TOK_IDF, currscope, 1);
		if (!(df->df_kind & (D_TYPE | D_HTYPE | D_ERROR))) {
			error("identifier \"%s\" not a type", df->df_idf->id_text);
		}
		*(f->fo_ptyp) = df->df_type;
		fo = f->next;
		free_forwards(f);
	}
	dot = savetok;
}
