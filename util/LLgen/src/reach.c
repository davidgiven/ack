/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * reach.c
 * Determine which nonterminals are reachable, and also check that they
 * are all defined.
 */

# include "tunable.h"
# include "types.h"
# include "extern.h"
# include "io.h"
# include "assert.h"

# ifndef NORCSID
static string rcsid8 = "$Header$";
# endif

/* In this file the following routines are defined: */
extern co_reach();
STATIC reachable();
STATIC reachwalk();

co_reach() {
	/*
	 * Check for undefined or unreachable nonterminals.
	 * An undefined nonterminal is a fatal error!
	 */
	register p_nont		p;
	register p_start	st;
	register p_file		x = files;
	register int		*s;

	/* Check for undefined nonterminals */
	for (p = nonterms; p < maxnt; p++) {
		if (! p->n_rule) {
			f_input = p->n_string;
			error(p->n_lineno,"nonterminal %s not defined",
				(min_nt_ent + (p - nonterms))->h_name);
		}
	}
	/*
	 * Walk the grammar rules, starting with the startsymbols
	 * Mark the nonterminals that are encountered with the flag
	 * REACHABLE, and walk their rules, if not done before
	 */
	for (st = start; st; st = st->ff_next) reachable(st->ff_nont);
	/*
	 * Now check for unreachable nonterminals
	 */
	for (; x->f_end < maxorder; x++) {
	    f_input = x->f_name;
	    for (s = x->f_start; s <= x->f_end; s++) {
		p = &nonterms[*s];
		if (! (p->n_flags & REACHABLE)) {
			error(p->n_lineno,"nonterminal %s unreachable",
				(min_nt_ent + (p - nonterms))->h_name);
		}
	    }
	}
}

STATIC
reachable(p) register p_nont p; {
	/*
	 * Enter the fact that p is reachable, and look for implications
	 */
	if (! (p->n_flags & REACHABLE)) {
		p->n_flags |= REACHABLE;
		/*
		 * Now walk its grammar rule
		 */
		if (p->n_rule) reachwalk(p->n_rule);
	}
}

STATIC
reachwalk(p) register p_gram p; {
	/*
	 * Walk through rule p, looking for nonterminals.
	 * The nonterminals found are entered as reachable
	 */

	for (;;) {
		switch(g_gettype(p)) {
		  case ALTERNATION :
			reachwalk(((p_link) pentry[g_getcont(p)])->l_rule);
			break;
		  case TERM :
			reachwalk(((p_term) pentry[g_getcont(p)])->t_rule);
			break;
		  case NONTERM :
			reachable(&nonterms[g_getnont(p)]);
			break;
		  case EORULE :
			return;
		}
		p++;
	}
}
