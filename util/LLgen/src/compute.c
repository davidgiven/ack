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
 * compute.c
 * Defines routines to compute FIRST, FOLLOW etc.
 * Also checks the continuation grammar from the specified grammar.
 */

# include "types.h"
# include "tunable.h"
# include "extern.h"
# include "sets.h"
# include "assert.h"
# ifndef NDEBUG
# include "io.h"
# endif

# ifndef NORCSID
static string rcsid2 = "$Header$";
# endif

p_set		setalloc();

/* Defined in this file : */
extern createsets();
STATIC walk();
extern co_empty();
extern empty();
extern co_first();
STATIC first();
extern co_follow();
STATIC follow();
extern co_symb();
STATIC co_dirsymb();
STATIC co_others();
STATIC do_checkdefault();
STATIC checkdefault();
extern co_contains();
STATIC do_contains();
STATIC contains();
extern co_safes();
STATIC int do_safes();

createsets() {
	/*
	 * Allocate space for the sets
	 */
	register p_nont p;

	for (p = nonterms; p < maxnt; p++) {
		p->n_first = setalloc(setsize);
		p->n_follow = setalloc(setsize);
		walk(p->n_rule);
	}
}

STATIC
walk(p) register p_gram p; {
	/*
	 * Walk through the grammar rule p, allocating sets
	 */

	for (;;) {
		switch (g_gettype(p)) {
		  case TERM : {
			register p_term q;

			q = (p_term) pentry[g_getcont(p)];
			q->t_first = setalloc(setsize);
			q->t_follow = setalloc(setsize);
			walk(q->t_rule);
			break; }
		  case ALTERNATION : {
			register p_link l;

			l = (p_link) pentry[g_getcont(p)];
			l->l_symbs = setalloc(setsize);
			walk(l->l_rule);
			break; }
		  case EORULE :
			return;
		}
		p++;
	}
}

co_empty() {
	/* 
	 * Which nonterminals produce the empty string ?
	 */
	register int	change;
	register p_nont	p;

	change = 1;
	while (change) {
		change = 0;
		for (p=nonterms; p < maxnt; p++) {
			if ((!(p->n_flags & EMPTY)) && empty(p->n_rule)) {
				p->n_flags |= EMPTY;
				change = 1;
			}
		}
	}
}

empty(p) register p_gram p; {
	/*
	 * Does the rule pointed to by p produce empty ?
	 */

	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE : 
			return 1;
		  case TERM :  {
			register p_term q;

			q = (p_term) pentry[g_getcont(p)];
			if (r_getkind(&(q->t_reps)) == STAR
			    || r_getkind(&(q->t_reps)) == OPT
			    || empty(q->t_rule) ) break;
			return 0; }
		  case ALTERNATION :
			if (empty(((p_link)pentry[g_getcont(p)])->l_rule)) {
				return 1;
			}
			if (g_gettype(p+1) == EORULE) return 0;
			break;
		  case NONTERM :
			if (nonterms[g_getnont(p)].n_flags & EMPTY) {
				break;
			}
			/* Fall through */
		  case TERMINAL :
			return 0;
		}
		p++;
	}
}

co_first() {
	/*
	 * Compute the FIRST set for each nonterminal
	 */

	register p_nont	p;
	register int	change;

	change = 1;
	while (change) {
		change = 0;
		for (p = nonterms; p < maxnt; p++) {
			if (first(p->n_first,p->n_rule,0)) change = 1;
		}
	}
}

STATIC
first(setp,p,flag) p_set setp; register p_gram p; {
	/*
	 * Compute the FIRST set of rule p.
	 * If flag = 0, also the first sets for terms and alternations in
	 * the rule p are computed.
	 * The FIRST set is put in setp.
	 * return 1 if any of the sets changed
	 */
	register	s;	/* Will gather return value */
	int		noenter;/* when set, unables entering of elements into
			 	 * setp. This is necessary to walk through the
			 	 * rest of rule p.
				 */

	s = 0;
	noenter = 0;
	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			return s;
		  case TERM : {
			register p_term q;

			q = (p_term) pentry[g_getcont(p)];
			if (flag == 0) s |= first(q->t_first,q->t_rule,0);
			if (!noenter) s |= setunion(setp,q->t_first,setsize);
			p++;
			if (r_getkind(&(q->t_reps)) == STAR
			    || r_getkind(&(q->t_reps)) == OPT
			    || empty(q->t_rule) ) continue;
			break; }
		  case ALTERNATION : {
			register p_link l;

			l = (p_link) pentry[g_getcont(p)];
			if (flag == 0) s |= first(l->l_symbs,l->l_rule,0);
			if (noenter == 0) {
				s |= setunion(setp,l->l_symbs,setsize);
			}
			if (g_gettype(p+1) == EORULE) return s;
			}
			/* Fall Through */
		  case ACTION :
			p++;
			continue;
		  case TERMINAL :
			if ((noenter == 0) && !IN(setp,g_getcont(p))) {
				s = 1;
				PUTIN(setp,g_getcont(p));
			}
			p++;
			break;
		  case NONTERM : {
			register p_nont n;

			n = &nonterms[g_getnont(p)];
			if (noenter == 0)  {
				s |= setunion(setp,n->n_first,setsize);
				if (ntneeded && ! NTIN(setp,n-nonterms)) {
					s = 1;
					NTPUTIN(setp,n-nonterms);
				}
			}
			p++;
			if (n->n_flags & EMPTY) continue;
			break; }
		}
		if (flag == 0) {
			noenter = 1;
			continue;
		}
		return s;
	}
}

co_follow() {
	/*
	 * Compute the follow set for each nonterminal
	 */

	register p_nont	p;
	register	change;
	register	i;
	p_start		st;

	/*
	 * First put EOFILE in the follow set of the start symbols
	 */
	for (st = start; st; st = st->ff_next) PUTIN(st->ff_nont->n_follow,0);
	change = 1;
	i = 1;
	while (change) {
		change = 0;
		for (p = nonterms; p < maxnt; p++) {
			if (follow(p->n_follow,p->n_rule,i)) change = 1;
		}
		i = 0;
	}
}

STATIC
follow(setp,p,flag) p_set setp; register p_gram p; {
	/*
	 * setp is the follow set for the rule p.
	 * Compute the follow sets in the rule p from this set.
	 * Return 1 if any set changed
	 * flag controls the use of "first" in the computation.
	 * It should be 1 the first time a rule is done, 0 otherwise.
	 */
	register	s;	/* Will gather return value */

	s = 0;
	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			return s;
		  case TERM : {
			register p_term q;

			q = (p_term) pentry[g_getcont(p)];
			if (empty(p+1)) {
				/*
				 * If what follows the term can be empty,
				 * everything that can follow the whole
				 * rule can also follow the term
				 */
				s |= setunion(q->t_follow,setp,setsize);
			}
			/*
			 * Everything that can start the rest of the rule
			 * can follow the term
			 */
			if (flag) s |= first(q->t_follow,p+1,1);
			if (r_getkind(&(q->t_reps)) == STAR
			    || r_getkind(&(q->t_reps)) == PLUS
			    || r_getnum(&(q->t_reps)) ) {
				/*
				 * If the term involves a repetition
				 * of possibly more than one,
				 * everything that can start the term
				 * can also follow it.
				 */
				s |= follow(q->t_first,q->t_rule,flag);
			}
			/*
			 * Now propagate the set computed sofar
			 */
			s |= follow(q->t_follow, q->t_rule,flag);
			break; }
		  case ALTERNATION :
			/*
			 * Just propagate setp
			 */
			s |= follow(setp,((p_link)pentry[g_getcont(p)])->l_rule,
					flag);
			break;
		  case NONTERM : {
			register p_nont n;

			n = &nonterms[g_getnont(p)];
			if (flag) s |= first(n->n_follow,p+1,1);
			if (empty(p+1)) {
				/*
				 * If the rest of p can produce empty,
				 * everything that follows p can follow
				 * the nonterminal
				 */
				s |= setunion(n->n_follow,setp,setsize);
			}
			break; }
		}
		p++;
	}
}

co_symb() {
	/*
	 * Compute the sets which determine which alternative to choose
	 * in case of a choice
	 * Also check the continuation grammar and see if rules do scan
	 * ahead.
	 */
	register p_nont p;

	for (p = nonterms; p < maxnt; p++) {
		co_dirsymb(p->n_follow,p->n_rule);
	}
	for (p = nonterms; p < maxnt; p++) {
		do_checkdefault(p);
	}
}

STATIC
co_dirsymb(setp,p) p_set setp; register p_gram p; {
	/*
	 * Walk the rule p, doing the work for alternations
	 */
	register p_gram s = 0;

	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			return;
		  case TERM : {
			register p_term q;

			q = (p_term) pentry[g_getcont(p)];
			co_dirsymb(q->t_follow,q->t_rule);
			break; }
		  case ALTERNATION : {
			register p_link l;
			/*
			 * Save first alternative
			 */
			if (!s) s = p;
			l = (p_link) pentry[g_getcont(p)];
			l->l_others = setalloc(setsize);
			co_dirsymb(setp,l->l_rule);
			if (empty(l->l_rule)) {
				/*
				 * If the rule can produce empty, everything
				 * that follows it can also start it
				 */
				setunion(l->l_symbs,setp,setsize);
			}
			if (g_gettype(p+1) == EORULE) {
				/*
				 * Every alternation is implemented as a
				 * choice between two alternatives :
				 * this one or one of the following.
				 * The l_others set will contain the starters
				 * of the other alternatives
				 */
				co_others(s);
				return;
			} }
		}
		p++;
	}
}

STATIC
co_others(p) register p_gram p; {
	/*
	 * compute the l_others-sets for the list of alternatives
	 * indicated by p
	 */
	register p_link l1,l2;

	l1 = (p_link) pentry[g_getcont(p)];
	p++;
	l2 = (p_link) pentry[g_getcont(p)];
	setunion(l1->l_others,l2->l_symbs,setsize);
	if (g_gettype(p+1) != EORULE) {
		/*
		 * First compute l2->l_others
		 */
		co_others(p);
		/*
		 * and then l1->l_others
		 */
		setunion(l1->l_others,l2->l_others,setsize);
	}
}

STATIC
do_checkdefault(p) register p_nont p; {
	/*
	 * check the continuation rule for nonterminal p, unless
	 * this is already being(!) done
	 */
	if (p->n_flags & BUSY) {
		/*
		 * Error situation, recursion in continuation grammar
		 */
		p->n_flags ^= (RECURSIVE|BUSY);
		return; 
	}
	if (p->n_flags & CONTIN) {
		/*
		 * Was already done
		 */
		return;
	}
	/*
	 * Now mark it as busy, and check the rule
	 */
	p->n_flags |= BUSY;
	checkdefault(p->n_rule);
	/*
	 * Now release the busy mark, and mark it as done
	 */
	p->n_flags ^= (CONTIN|BUSY);
	return;
}

STATIC
checkdefault(p) register p_gram p; {
	/*
	 * Walk grammar rule p, checking the continuation grammar
	 */
	register p_link l;
	register p_term q;

	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			return;
		  case ALTERNATION :
			l = (p_link) pentry[g_getcont(p)];
			if (l->l_flag & DEF) {
				/*
				 * This alternative belongs to the
				 * continuation grammar, so check it
				 */
				checkdefault(l->l_rule);
				return;
			}
			break;
		  case TERM :
			q = (p_term) pentry[g_getcont(p)];
			/*
			 * First check the rest of the rule
			 */
			checkdefault(p+1);
			/*
			 * Now check the term if it belongs to the
			 * continuation grammar
			 */
			if (r_getkind(&(q->t_reps))==FIXED ||
			    r_getkind(&(q->t_reps))==PLUS) {
				checkdefault(q->t_rule);
				return;
			}
			/*
			 * Here we have OPT or STAR
			 * Only in the continuation grammar if %persistent
			 */
			if (q->t_flags & PERSISTENT) {
				checkdefault(q->t_rule);
			}
			return;
		  case NONTERM :
			/*
			 * Check the continuation grammar for this nonterminal.
			 * Note that the nonterminal we are working on is
			 * marked as busy, so that direct or indirect recursion
			 * can be detected
			 */
			do_checkdefault(&nonterms[g_getnont(p)]);
			break;
		}
		p++;
	}
}

co_contains() {
	/*
	 * Compute the contains sets
	 */
	register p_nont p;
	register p_set dummy;

	for (p = nonterms; p < maxnt; p++) do_contains(p);
	dummy = setalloc(setsize);
# ifndef NDEBUG
	if (debug) fputs("Contains 1 done\n", stderr);
# endif
	free(dummy);
	for (p = nonterms; p < maxnt; p++) contains(p->n_rule, (p_set) 0);
# ifndef NDEBUG
	if (debug) fputs("Contains 2 done\n", stderr);
# endif
	dummy = setalloc(setsize);
	free(dummy);
}

STATIC
do_contains(n) register p_nont n; {
	/*
	 * Compute the total set of symbols that nonterminal n can
	 * produce 
	 */

	if (n->n_contains == 0) {
		n->n_contains = setalloc(setsize);
		contains(n->n_rule,n->n_contains);
		/*
		 * If the rule can produce empty, delete all symbols that
		 * can follow the rule as well as be in the rule.
		 * This is needed because the contains-set may only contain
		 * symbols that are guaranteed to be eaten by the rule!
		 * Otherwise, the generated parser may loop forever
		 */
		if (n->n_flags & EMPTY) {
			setminus(n->n_contains,n->n_follow,setsize);
		}
		/*
		 * But the symbols that can start the rule are always
		 * eaten
		 */
		setunion(n->n_contains,n->n_first,setsize);
	}
}

STATIC
contains(p,set) register p_gram p; register p_set set; {
	/*
	 * Does the real computation of the contains-sets
	 */

	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			return;
		  case TERM : {
			register p_term q;

			q = (p_term) pentry[g_getcont(p)];
			if ((q->t_flags & PERSISTENT) ||
			    r_getkind(&(q->t_reps)) == PLUS ||
			    r_getkind(&(q->t_reps)) == FIXED) {
				/*
				 * In these cases, the term belongs to the
				 * continuation grammar.
				 * Otherwise, q->t_contains is just
				 * q->t_first
				 */
				if (!q->t_contains) {
				    q->t_contains = setalloc(setsize);
				}
				contains(q->t_rule,q->t_contains);
				if (empty(q->t_rule)) {
					/*
					 * Same trouble as mentioned in the
					 * routine do_contains
					 */
					setminus(q->t_contains,q->t_follow,
						setsize);
				}
				setunion(q->t_contains,q->t_first,setsize);
			} else {
				contains(q->t_rule, (p_set) 0);
				q->t_contains = q->t_first;
			}
			if (set) setunion(set,q->t_contains,setsize);
			break; }
		  case NONTERM : {
			register p_nont n;

			n = &nonterms[g_getnont(p)];
			do_contains(n);
			if(set) setunion(set, n->n_contains,setsize);
			break; }
		  case ALTERNATION : {
			register p_link l;

			l = (p_link) pentry[g_getcont(p)];
			contains(l->l_rule,
				(l->l_flag & DEF) ? set : (p_set) 0);
			break; }
		  case TERMINAL : {
			register hulp;

			if (set) {
				hulp = g_getcont(p);
				assert(hulp < nterminals);
				PUTIN(set,hulp);
			}}
		}
		p++;
	}
}

static int change;

co_safes() {
	/*
	 * Compute the safety of each nonterminal and term.
	 * The safety gives an answer to the question whether a scan is done,
	 * and how it should be handled.
	 */

	register p_nont	p;
	register	i;
	register p_start st;

	for (p = nonterms; p < maxnt; p++) {
		/*
		 * Don't know anything yet
		 */
		setntsafe(p, NOSAFETY);
		setntout(p, NOSAFETY);
	}
	for (st = start; st; st = st->ff_next) {
		/*
		 * But start symbols are called with lookahead done
		 */
		p = st->ff_nont;
		setntsafe(p,SCANDONE);
	}
	change = 1;
	while (change) {
		change = 0;
		for (p = nonterms; p < maxnt; p++) {
			i = getntsafe(p);
			if (i == NOSAFETY) {
				continue;
			}
			i = do_safes(p->n_rule, i);
			if (getntout(p) != i) {
				change = 1;
				setntout(p, i);
			}
		}
	}
}

STATIC int
do_safes(p,safe) register p_gram p; {
	/*
	 * Walk the grammar rule, doing the computation described in the
	 * comment of the procedure above this one.
	 */
	register	retval;

	for (;;) {
		switch (g_gettype(p)) {
		  case ACTION:
		  	p++;
			continue;
		  case TERMINAL:
			safe = NOSCANDONE;
			break;
		  case TERM : {
			register p_term q;
			int i,rep;

			q = (p_term) pentry[g_getcont(p)];
			i = r_getnum(&(q->t_reps));
			rep = r_getkind(&(q->t_reps));
			retval = do_safes(q->t_rule,
			       t_safety(rep,i,q->t_flags&PERSISTENT,safe));
			if (retval != gettout(q)) {
				settout(q, retval);
			}
			safe = t_after(rep, i, gettout(q));
			break; }
		  case ALTERNATION : {
		  	register p_link l;
			int f, i;

			f = 1;
			while (g_gettype(p) == ALTERNATION) {
				l = (p_link) pentry[g_getcont(p)];
				if (safe > SAFE && (l->l_flag & DEF)) {
					i = do_safes(l->l_rule,SAFESCANDONE);
				}
				else	i = do_safes(l->l_rule,SAFE);
				if (f) retval = i;
				else if (i != retval) {
					if (i == NOSCANDONE ||
					    retval == NOSCANDONE) {
						retval = SCANDONE;
					}
					else if (i > retval) retval = i;
				}
				p++;
				f = 0;
			}
			return retval; }
		  case NONTERM : {
			register p_nont n;
			int nsafe, osafe;

			n = &nonterms[g_getnont(p)];
			nsafe = getntsafe(n);
			osafe = safe;
			safe = getntout(n);
			if (safe == NOSAFETY) safe = SCANDONE;
			if (osafe == nsafe) break;
			if (nsafe == NOSAFETY) {
				change = 1;
				setntsafe(n, osafe);
				break;
			}
			if (osafe == NOSCANDONE || nsafe == NOSCANDONE) {
				if (nsafe != SCANDONE) {
					change = 1;
					setntsafe(n, SCANDONE);
				}
				break;
			}
			if (osafe > nsafe) {
				setntsafe(n, osafe);
				change = 1;
			}
			break; }
		  case EORULE :
			return safe;
		}
		p++;
	}
}

t_safety(rep, count, persistent, safety) {

	switch(rep) {
	  default:
		assert(0);
	  case OPT:
		if (!persistent) return SAFE;
		if (safety < SAFESCANDONE) return safety;
		return SAFESCANDONE;
	  case STAR:
		if (persistent) return SAFESCANDONE;
		return SAFE;
	  case PLUS:
		if (safety == NOSCANDONE) safety = SCANDONE;
		if (persistent) {
			if (safety > SAFESCANDONE) return safety;
			return SAFESCANDONE;
		}
		if (safety > SAFE) return safety;
		return SAFE;
	  case FIXED:
		if (!count) {
			if (safety == NOSCANDONE) safety = SCANDONE;
			return safety;
		}
		return SCANDONE;
	}
	/* NOTREACHED */
}

t_after(rep, count, outsafety) {
	if (count == 0 && (rep == STAR || rep == PLUS)) {
		return SAFESCANDONE;
	}
	if (rep != FIXED) {
		if (outsafety <= SAFESCANDONE) return SAFESCANDONE;
		return SCANDONE;
	}
	return outsafety;
}
