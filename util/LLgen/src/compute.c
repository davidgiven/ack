/* Copyright (c) 1991 by the Vrije Universiteit, Amsterdam, the Netherlands.
 * For full copyright and restrictions on use see the file COPYING in the top
 * level of the LLgen tree.
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
# include "extern.h"
# include "sets.h"
# include "assert.h"
# include "io.h"

# ifndef NORCSID
static string rcsid = "$Id$";
# endif

p_set		get_set();
typedef struct lngth {
			/* Structure used to compute the shortest possible
			 * length of a terminal production of a rule.
			 * In case of a tie, the second field is used.
			 */
	int cnt;
	int val;
} t_length, *p_length;

/* Defined in this file : */
extern do_compute();
STATIC createsets();
STATIC walk();
STATIC co_trans();
STATIC int nempty();
extern empty();
STATIC int nfirst();
STATIC first();
STATIC int nfollow();
STATIC follow();
STATIC co_dirsymb();
STATIC co_others();
STATIC do_lengthcomp();
STATIC complength();
STATIC add();
STATIC int compare();
STATIC setdefaults();
STATIC do_contains();
STATIC contains();
STATIC int nsafes();
STATIC int do_safes();

do_compute() {
	/*
	 * Does all the work, by calling other routines (divide and conquer)
	 */
	register p_nont	p;
	register p_start st;

	createsets();
	co_trans(nempty);	/* Which nonterminals produce empty? */
	co_trans(nfirst);	/* Computes first sets */
	/*
	 * Compute FOLLOW sets.
	 * First put EOFILE in the follow set of the start nonterminals.
	 */
	for (st = start; st; st = st->ff_next) {
		p = &nonterms[st->ff_nont];
		PUTIN(p->n_follow,0);
	}
	co_trans(nfollow);
	/*
	 * Compute the sets which determine which alternative to choose
	 * in case of a choice
	 */
	for (p = nonterms; p < maxnt; p++) {
		co_dirsymb(p->n_follow,p->n_rule);
	}
	/*
	 * Compute the minimum length of productions of nonterminals,
	 * and then determine the default choices
	 */
	do_lengthcomp();
	/*
	 * Compute the contains sets
	 */
	for (p = nonterms; p < maxnt; p++) do_contains(p);
	for (p = nonterms; p < maxnt; p++) contains(p->n_rule, (p_set) 0);
	/*
	 * Compute the safety of each nonterminal and term.
	 * The safety gives an answer to the question whether a scan is done,
	 * and how it should be handled.
	 */
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
		p = &nonterms[st->ff_nont];
		setntsafe(p,SCANDONE);
	}
	co_trans(nsafes);
# ifndef NDEBUG
	if (debug) {
		fputs("Safeties:\n", stderr);
		for (p = nonterms; p < maxnt; p++) {
			fprintf(stderr, "%s\t%d\t%d\n",
				p->n_name,
				getntsafe(p),
				getntout(p));
		}
	}
# endif
}

STATIC
createsets() {
	/*
	 * Allocate space for the sets. Also determine which files use
	 * which nonterminals, and determine which nonterminals can be
	 * made static.
	 */
	register p_nont p;
	register p_file f;
	register p_start st;
	register int i;
	int n = NINTS(NBYTES(nnonterms));
	p_mem alloc();

	for (f = files; f < maxfiles; f++) {
		register p_set s;
		f->f_used = s = (p_set) alloc((unsigned)n*sizeof(*(f->f_used)));
		for (i = n; i; i--) *s++ = 0;
		for (i = f->f_nonterminals; i != -1; i = p->n_next) {
			p = &nonterms[i];
			p->n_flags |= GENSTATIC;
			p->n_first = get_set();
			p->n_follow = get_set();
			walk(f->f_used, p->n_rule);
		}
	}
	for (f = files; f < maxfiles; f++) {
		for (i = f->f_nonterminals; i != -1; i = p->n_next) {
			register p_file f2;

			p = &nonterms[i];
			for (f2 = files; f2 < maxfiles; f2++) {
				if (f2 != f && IN(f2->f_used, i)) {
					p->n_flags &= ~GENSTATIC;
				}
			}
		}
	}
	for (st = start; st; st = st->ff_next) {
		nonterms[st->ff_nont].n_flags &= ~GENSTATIC;
	}
}

STATIC
walk(u, p) p_set u; register p_gram p; {
	/*
	 * Walk through the grammar rule p, allocating sets
	 */

	for (;;) {
		switch (g_gettype(p)) {
		  case TERM : {
			register p_term q;

			q = g_getterm(p);
			q->t_first = get_set();
			q->t_follow = get_set();
			walk(u, q->t_rule);
			break; }
		  case ALTERNATION : {
			register p_link l;

			l = g_getlink(p);
			l->l_symbs = get_set();
			l->l_others = get_set();
			walk(u, l->l_rule);
			break; }
		  case NONTERM : {
			register int i = g_getcont(p);

			PUTIN(u, i);
			break; }
		  case EORULE :
			return;
		}
		p++;
	}
}

STATIC
co_trans(fc) int (*fc)(); {
	register p_nont p;
	register int change;

	do {
		change = 0;
		for (p = nonterms; p < maxnt; p++) {
			if ((*fc)(p)) change = 1;
		}
	} while (change);
}

STATIC int
nempty(p) register p_nont p; {
	if (!(p->n_flags & EMPTY) && empty(p->n_rule)) {
		p->n_flags |= EMPTY;
		return 1;
	}
	return 0;
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

			q = g_getterm(p);
			if (r_getkind(q) == STAR
			    || r_getkind(q) == OPT
			    || empty(q->t_rule) ) break;
			return 0; }
		  case ALTERNATION :
			if (empty(g_getlink(p)->l_rule)) {
				return 1;
			}
			if (g_gettype(p+1) == EORULE) return 0;
			break;
		  case NONTERM :
			if (nonterms[g_getcont(p)].n_flags & EMPTY) {
				break;
			}
			/* Fall through */
		  case LITERAL :
		  case TERMINAL :
			return 0;
		}
		p++;
	}
}

STATIC int
nfirst(p) register p_nont p; {
	return first(p->n_first, p->n_rule, 0);
}

STATIC
first(setp,p,flag) p_set setp; register p_gram p; {
	/*
	 * Compute the FIRST set of rule p.
	 * If flag = 0, also the first sets for terms and alternations in
	 * the rule p are computed.
	 * The FIRST set is put in setp.
	 * return 1 if the set refered to by "setp" changed
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

			q = g_getterm(p);
			if (flag == 0) {
				if (first(q->t_first,q->t_rule,0))/*nothing*/;
			}
			if (!noenter) s |= setunion(setp,q->t_first);
			p++;
			if (r_getkind(q) == STAR ||
			    r_getkind(q) == OPT ||
			    empty(q->t_rule)) continue;
			break; }
		  case ALTERNATION : {
			register p_link l;

			l = g_getlink(p);
			if (flag == 0) {
				if (first(l->l_symbs,l->l_rule,0))/*nothing*/;
			}
			if (noenter == 0) {
				s |= setunion(setp,l->l_symbs);
			}
			if (g_gettype(p+1) == EORULE) return s;
			}
			/* Fall Through */
		  case ACTION :
			p++;
			continue;
		  case LITERAL :
		  case TERMINAL :
			if ((noenter == 0) && !IN(setp,g_getcont(p))) {
				s = 1;
				PUTIN(setp,g_getcont(p));
			}
			p++;
			break;
		  case NONTERM : {
			register p_nont n;

			n = &nonterms[g_getcont(p)];
			if (noenter == 0)  {
				s |= setunion(setp,n->n_first);
				if (ntneeded) NTPUTIN(setp,g_getcont(p));
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

STATIC int
nfollow(p) register p_nont p; {
	return follow(p->n_follow, p->n_rule);
}

STATIC
follow(setp,p) p_set setp; register p_gram p; {
	/*
	 * setp is the follow set for the rule p.
	 * Compute the follow sets in the rule p from this set.
	 * Return 1 if a follow set of a nonterminal changed.
	 */
	register	s;	/* Will gather return value */

	s = 0;
	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			return s;
		  case TERM : {
			register p_term q;

			q = g_getterm(p);
			if (empty(p+1)) {
				/*
				 * If what follows the term can be empty,
				 * everything that can follow the whole
				 * rule can also follow the term
				 */
				s |= setunion(q->t_follow,setp);
			}
			/*
			 * Everything that can start the rest of the rule
			 * can follow the term
			 */
			s |= first(q->t_follow,p+1,1);
			if (r_getkind(q) == STAR ||
			    r_getkind(q) == PLUS ||
			    r_getnum(q) ) {
				/*
				 * If the term involves a repetition
				 * of possibly more than one,
				 * everything that can start the term
				 * can also follow it.
				 */
				s |= follow(q->t_first,q->t_rule);
			}
			/*
			 * Now propagate the set computed sofar
			 */
			s |= follow(q->t_follow, q->t_rule);
			break; }
		  case ALTERNATION :
			/*
			 * Just propagate setp
			 */
			s |= follow(setp,g_getlink(p)->l_rule);
			break;
		  case NONTERM : {
			register p_nont n;

			n = &nonterms[g_getcont(p)];
			s |= first(n->n_follow,p+1,1);
			if (empty(p+1)) {
				/*
				 * If the rest of p can produce empty,
				 * everything that follows p can follow
				 * the nonterminal
				 */
				s |= setunion(n->n_follow,setp);
			}
			break; }
		}
		p++;
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

			q = g_getterm(p);
			co_dirsymb(q->t_follow,q->t_rule);
			break; }
		  case ALTERNATION : {
			register p_link l;
			/*
			 * Save first alternative
			 */
			if (!s) s = p;
			l = g_getlink(p);
			co_dirsymb(setp,l->l_rule);
			if (empty(l->l_rule)) {
				/*
				 * If the rule can produce empty, everything
				 * that follows it can also start it
				 */
				setunion(l->l_symbs,setp);
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

	l1 = g_getlink(p);
	p++;
	l2 = g_getlink(p);
	setunion(l1->l_others,l2->l_symbs);
	if (g_gettype(p+1) != EORULE) {
		/*
		 * First compute l2->l_others
		 */
		co_others(p);
		/*
		 * and then l1->l_others
		 */
		setunion(l1->l_others,l2->l_others);
	}
}

static p_length length;
# define INFINITY 32767

STATIC
ncomplength(p)
	register p_nont p;
{
	register p_length pl = &length[p - nonterms];
	int x = pl->cnt;

	pl->cnt = -1;
	complength(p->n_rule, pl);
	return pl->cnt < INFINITY && x == INFINITY;
}

STATIC
do_lengthcomp() {
	/*
	 * Compute the minimum length of a terminal production for each
	 * nonterminal.
	 * This length consists of two fields: the number of terminals,
	 * and a number that is composed of 
	 * - the number of this alternative
	 * - a crude measure of the number of terms and nonterminals in the
	 *   production of this shortest string.
	 */
	register p_length pl;
	register p_nont p;
	p_mem alloc();

	length = (p_length) alloc((unsigned) (nnonterms * sizeof(*length)));
	for (pl = &length[nnonterms-1]; pl >= length; pl--) {
		pl->val = pl->cnt = INFINITY;
	}

	co_trans(ncomplength);

	pl = length;
	for (p = nonterms; p < maxnt; p++, pl++) {
		if (pl->cnt == INFINITY) {
			p->n_flags |= RECURSIVE;
		}
		setdefaults(p->n_rule);
	}
	free ((p_mem) length);
}

STATIC
complength(p,le) register p_gram p; p_length le; {
	/*
	 * Walk grammar rule p, computing minimum lengths
	 */
	register p_link l;
	register p_term q;
	t_length i;
	t_length X;
	int cnt = 0;

	X.cnt = 0;
	X.val = 0;
	for (;;) {
		switch (g_gettype(p)) {
		  case LITERAL :
		  case TERMINAL :
			add(&X, 1, 0);
			break;
		  case ALTERNATION :

			X.cnt = INFINITY;
			X.val = INFINITY;
			while (g_gettype(p) != EORULE) {
				cnt++;
				l = g_getlink(p);
				complength(l->l_rule,&i);
				i.val += cnt;
				if (l->l_flag & DEF) {
					X = i;
					break;
				}
				if (compare(&i, &X) < 0) {
					X = i;
				}
				p++;
			}
			/* Fall through */
		  case EORULE :
			le->cnt = X.cnt;
			le->val = X.val;
			return;
		  case TERM : {
			register int rep;

			q = g_getterm(p);
			rep = r_getkind(q);
			X.val += 1;
			if ((q->t_flags&PERSISTENT) || 
			    rep==FIXED || rep==PLUS) {
				complength(q->t_rule,&i);
				add(&X, i.cnt, i.val);
				if (rep == FIXED && r_getnum(q) > 0) {
					for (rep = r_getnum(q) - 1;
					     rep > 0; rep--) {
						add(&X, i.cnt, i.val);
					}
				}
			}
			break; }
		  case NONTERM : {
			int nn = g_getcont(p);
			register p_length pl = &length[nn];
			int x = pl->cnt;

			if (x == INFINITY) {
				pl->cnt = -1;
				complength(nonterms[nn].n_rule,pl);
				x = pl->cnt;
			}
			else if (x == -1) x = INFINITY;
			add(&X, x, pl->val);
			X.val += 1;
			}
		}
		p++;
	}
}

STATIC
add(a, c, v) register p_length a; {

	if (a->cnt == INFINITY || c == INFINITY) {
		a->cnt = INFINITY;
		return;
	}
	a->val += v;
	a->cnt += c;
}

STATIC int
compare(a, b) register p_length a, b; {
	if (a->cnt != b->cnt) return a->cnt - b->cnt;
	return a->val - b->val;
}

STATIC
setdefaults(p) register p_gram p; {
	for (;;) {
		switch(g_gettype(p)) {
		  case EORULE:
			return;
		  case TERM:
			setdefaults(g_getterm(p)->t_rule);
			break;
		  case ALTERNATION: {
			register p_link l, l1;
			int temp = 0, temp1, cnt = 0;
			t_length count, i;

			count.cnt = INFINITY;
			count.val = INFINITY;
			l1 = g_getlink(p);
			do {
				cnt++;
				l = g_getlink(p);
				complength(l->l_rule,&i);
				i.val += cnt;
				if (l->l_flag & DEF) temp = 1;
				temp1 = compare(&i, &count);
				if (temp1 < 0 ||
				    (temp1 == 0 && l1->l_flag & AVOIDING)) {
					l1 = l;
					count = i;
				}
				setdefaults(l->l_rule);
				p++;
			} while (g_gettype(p) != EORULE);
			if (!temp) {
				/* No user specified default */
				l1->l_flag |= DEF;
			}
			return; }
		}
		p++;
	}
}

STATIC
do_contains(n) register p_nont n; {
	/*
	 * Compute the total set of symbols that nonterminal n can
	 * produce 
	 */

	if (n->n_contains == 0) {
		n->n_contains = get_set();
		contains(n->n_rule,n->n_contains);
		/*
		 * If the rule can produce empty, delete all symbols that
		 * can follow the rule as well as be in the rule.
		 * This is needed because the contains-set may only contain
		 * symbols that are guaranteed to be eaten by the rule!
		 * Otherwise, the generated parser may loop forever
		 */
		if (n->n_flags & EMPTY) {
			setminus(n->n_contains,n->n_follow);
		}
		/*
		 * But the symbols that can start the rule are always
		 * eaten
		 */
		setunion(n->n_contains,n->n_first);
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
			int rep;

			q = g_getterm(p);
			rep = r_getkind(q);
			if ((q->t_flags & PERSISTENT) ||
			    rep == PLUS || rep == FIXED) {
				/*
				 * In these cases, the term belongs to the
				 * continuation grammar.
				 * Otherwise, q->t_contains is just
				 * q->t_first
				 */
				if (!q->t_contains) {
				    q->t_contains = get_set();
				}
				contains(q->t_rule,q->t_contains);
				if (rep != FIXED || empty(q->t_rule)) {
					setminus(q->t_contains,q->t_follow);
				}
				setunion(q->t_contains,q->t_first);
			} else {
				contains(q->t_rule, (p_set) 0);
				q->t_contains = q->t_first;
			}
			if (set) setunion(set,q->t_contains);
			break; }
		  case NONTERM : {
			register p_nont n;

			n = &nonterms[g_getcont(p)];
			do_contains(n);
			if (set) {
				setunion(set, n->n_contains);
				if (ntneeded) NTPUTIN(set, g_getcont(p));
			}
			break; }
		  case ALTERNATION : {
			register p_link l;

			l = g_getlink(p);
			contains(l->l_rule,
				(l->l_flag & DEF) ? set : (p_set) 0);
			break; }
		  case LITERAL :
		  case TERMINAL : {
			register hulp;

			if (set) {
				hulp = g_getcont(p);
				assert(hulp < ntokens);
				PUTIN(set,hulp);
			}}
		}
		p++;
	}
}

STATIC int nsafes(p) register p_nont p; {
	int	ch;
	register int i;

	ch = 0;
	i = getntsafe(p);
	if (i != NOSAFETY) {
		i = do_safes(p->n_rule, i, &ch);
		if (i < SCANDONE) i = SCANDONE;
		/* After a nonterminal, we only know whether a scan was done
		   or not
		*/
		if (getntout(p) != i) {
			ch = 1;
			setntout(p,i);
		}
	}
	return ch;
}

STATIC int
do_safes(p,safe,ch) register p_gram p; register int *ch; {
	/*
	 * Walk the grammar rule, doing the computation described in the
	 * comment of the procedure above this one.
	 */
	int	retval;

	for (;;) {
		switch (g_gettype(p)) {
		  case ACTION:
		  	p++;
			continue;
		  case LITERAL:
		  case TERMINAL:
			safe = NOSCANDONE;
			break;
		  case TERM : {
			register p_term q;
			int i,rep;

			q = g_getterm(p);
			i = r_getnum(q);
			rep = r_getkind(q);
			retval = do_safes(q->t_rule,
			       t_safety(rep,i,q->t_flags&PERSISTENT,safe),ch);
			settout(q, retval);
			safe = t_after(rep, i, retval);
			break; }
		  case ALTERNATION : {
		  	register p_link l;
			register int i;

			retval = -1;
			while (g_gettype(p) == ALTERNATION) {
				l = g_getlink(p);
				if (safe > SAFE && (l->l_flag & DEF)) {
					i = do_safes(l->l_rule,SAFESCANDONE,ch);
				}
				else	i = do_safes(l->l_rule,SAFE,ch);
				if (retval == -1) retval = i;
				else if (i != retval) {
					if (i == NOSCANDONE ||
					    retval == NOSCANDONE) {
						retval = SCANDONE;
					}
					else if (i > retval) retval = i;
				}
				p++;
			}
			return retval; }
		  case NONTERM : {
			register p_nont n;
			register int nsafe, osafe;

			n = &nonterms[g_getcont(p)];
			nsafe = getntsafe(n);
			osafe = safe;
			safe = getntout(n);
			if (safe == NOSAFETY) safe = SCANDONE;
			if (osafe == nsafe) break;
			if (nsafe == NOSAFETY) {
				*ch = 1;
				setntsafe(n, osafe);
				break;
			}
			if (osafe == NOSCANDONE || nsafe == NOSCANDONE) {
				if (nsafe != SCANDONE) {
					*ch = 1;
					setntsafe(n, SCANDONE);
				}
				break;
			}
			if (osafe > nsafe) {
				setntsafe(n, osafe);
				*ch = 1;
			}
			break; }
		  case EORULE :
			return safe;
		}
		p++;
	}
}

t_safety(rep, count, persistent, safety) {

	if (safety == NOSCANDONE) safety = SCANDONE;
	switch(rep) {
	  default:
		assert(0);
	  case OPT:
		if (!persistent || safety < SAFESCANDONE) return SAFE;
		return SAFESCANDONE;
	  case STAR:
		if (persistent) return SAFESCANDONE;
		return SAFE;
	  case PLUS:
		if (persistent) {
			if (safety > SAFESCANDONE) return safety;
			return SAFESCANDONE;
		}
		return safety;
	  case FIXED:
		if (!count) return safety;
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
