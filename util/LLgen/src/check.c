/* Copyright (c) 1991 by the Vrije Universiteit, Amsterdam, the Netherlands.
 * All rights reserved.
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * check.c
 * Several routines to perform checks and printouts
 */

# include "types.h"
# include "extern.h"
# include "io.h"
# include "sets.h"
# include "assert.h"

# ifndef NORCSID
static string rcsid1 = "$Header$";
# endif


static string	c_first = "> firstset   ";
static string	c_contains = "> containset ";
static string	c_follow = "> followset  ";
p_set		setalloc();
static int	level;

/* In this file are defined : */
extern conflchecks();
STATIC prline();
STATIC printset();
STATIC int check();
STATIC moreverbose();
STATIC prrule();
STATIC cfcheck();
STATIC resolve();
STATIC propagate();
STATIC spaces();

conflchecks() {
	/*
	 * Check for conflicts, that is,
	 * in a repeating term, the FIRST and FOLLOW must be disjunct,
	 * unless there is a disambiguating condition.
	 * in an alternation, the sets that determine the direction to take,
	 * must be disjunct.
	 */
	register p_nont	p;
	register int s;
	p_file		x = files;

	f_input = x->f_name;
	if (verbose >= 3) {
		for (p = nonterms; p < maxnt; p++) p->n_flags |= VERBOSE;
	}
	if (verbose) {
		if ((fout = fopen(f_out,"w")) == NULL) fatal(1,e_noopen,f_out);
	}
	/*
	 * Check the rules in the order in which they are declared,
	 * and input file by input file, to give proper error messages
	 */
	for (; x < maxfiles; x++) {
	    f_input = x->f_name;
	    for (s = x->f_nonterminals; s != -1; s = p->n_next) {
		p = &nonterms[s];
	        if (check(p->n_rule)) p->n_flags |= VERBOSE;
	    }
	}
	for (x = files; x < maxfiles; x++) {
	    f_input = x->f_name;
	    for (s = x->f_nonterminals; s != -1; s = p->n_next) {
		p = &nonterms[s];
		if (p->n_flags & RECURSIVE) {
			error(p->n_lineno,
				"Recursion in default for nonterminal %s",
				p->n_name);
		}
		/*
		 * If a printout is needed for this rule in
		 * LL.output, just do it
		 */
		if (verbose && (p->n_flags & VERBOSE)) {
			fprintf(fout,"\n%s :\n",p->n_name);
			printset(p->n_first,c_first);
			printset(p->n_contains,c_contains);
			printset(p->n_follow,c_follow);
			fprintf(fout,"> rule%s\n\t",
				p->n_flags&EMPTY ? "\t(EMPTY producing)" : "");
			level = 8;
			prrule(p->n_rule);
			level = 0;
			prline("\n");
		}
		/*
		 * Now, the conflicts may be resolved
		 */
		resolve(p->n_rule);
	    }
	}
	if (verbose) fclose(fout);
}

STATIC
prline(s) char *s; {
	fputs(s, fout);
	spaces();
}

STATIC
printset(p,s) register p_set p; string s; {
	/*
	 * Print the elements of a set
	 */
	register int	i;
	register int	j;
	register p_token pt;
	string		name;
	int		k;
	int		hulp;

	k = strlen(s) + 2 + level;
	/*
	 * k contains relative level of indentation
	 */
	fprintf(fout,"%s{ ",s);
	j = k;
	/*
	 * j will gather the total length of the line
	 */
	for (i = 0, pt = tokens; i < ntokens; i++,pt++) {
		if (IN(p,i)) {
			hulp = strlen(pt->t_string)+1;
			if (pt->t_tokno < 0400) hulp += 2;
			if ((j += hulp) >= 78) {
				/*
				 * Line becoming too long
				 */
				j = k+hulp;
				prline("\n");
				fprintf(fout,">%*c",k - level - 1,' ');
			}
			fprintf(fout, pt->t_tokno<0400 ? "'%s' " : "%s ",pt->t_string);
		}
	}
	if (ntprint) for (i = 0; i < nnonterms; i++) {
		/*
		 * Nonterminals in the set must also be printed
		 */
		if (NTIN(p,i)) {
			name = nonterms[i].n_name;
			hulp = strlen(name) + 3;
			if ((j += hulp) >= 78) {
				j = k + hulp;
				prline("\n");
				fprintf(fout,">%*c",k - level - 1,' ');
			}
			fprintf(fout,"<%s> ",name);
		}
	}
	prline("}\n");
}

STATIC int
check(p) register p_gram p; {
	/*
	 * Search for conflicts in a grammar rule.
	 */
	register p_set	temp;
	register int retval;

	retval = 0;
	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			return retval;
		  case NONTERM : {
			register p_nont n;

			n = &nonterms[g_getcont(p)];
			if (g_getnpar(p) != getntparams(n)) {
			    error(p->g_lineno,
			        "Call of %s: parameter count mismatch",
				n->n_name);
			}
			break; }
		  case TERM : {
			register p_term q;

			q = g_getterm(p);
			retval |= check(q->t_rule);
			if (r_getkind(q) == FIXED) break;
			if (setempty(q->t_first)) {
				q->t_flags |= EMPTYFIRST;
				retval = 1;
				error(p->g_lineno, "No symbols in term");
			}
			if (empty(q->t_rule)) {
				q->t_flags |= EMPTYTERM;
				retval = 1;
				error(p->g_lineno, "Term with variable repetition count produces empty");
			}
			temp = setalloc();
			setunion(temp,q->t_first);
			if (!setintersect(temp,q->t_follow)) {
				/* 
			 	 * q->t_first * q->t_follow != EMPTY
			 	 */
				if (!(q->t_flags & RESOLVER)) {
			  		/*
			   		 * No conflict resolver
			   		 */
					error(p->g_lineno,
						"Repetition conflict");
					retval = 1;
					moreverbose(temp);
				}
			}
			else {
				if (q->t_flags & RESOLVER) {
					q->t_flags |= NOCONF;
					warning(p->g_lineno,
						"%%while without conflict");
				}
			}
			free((p_mem) temp);
			break; }
		  case ALTERNATION : {
			register p_link l;

			l = g_getlink(p);
			temp = setalloc();
			setunion(temp,l->l_symbs);
			if(!setintersect(temp,l->l_others)) {
				/*
				 * temp now contains the conflicting
				 * symbols
				 */
				if (!(l->l_flag & (COND|PREFERING|AVOIDING))) {
					error(p->g_lineno,
"Alternation conflict");
					retval = 1;
					moreverbose(temp);
				} 
			} else {
				if (l->l_flag & (COND|PREFERING|AVOIDING)) {
					l->l_flag |= NOCONF;
					warning(p->g_lineno,
"Conflict resolver without conflict");
				}
			}
			if (l->l_flag & PREFERING) propagate(l->l_symbs,p+1);
			free( (p_mem) temp);
			retval |= check(l->l_rule);
			break; }
		}
		p++;
	}
}

STATIC
moreverbose(t) register p_set t; {
	/*
	 * t points to a set containing conflicting symbols and pssibly
	 * also containing nonterminals.
	 * Take care that a printout will be prepared for these nonterminals
	 */
	register int i;
	register p_nont p;

	if (verbose == 2) for (i = 0, p = nonterms; i < nnonterms; i++, p++) {
		if (NTIN(t,i)) p->n_flags |= VERBOSE;
	}
}

STATIC
prrule(p) register p_gram p; {
	/*
	 * Create a verbose printout of grammar rule p
	 */
	register FILE	*f;
	int		present = 0;
	int		firstalt = 1;

	f = fout;
	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			fputs("\n",f);
			return;
		  case TERM : {
			register p_term	q;
			register int	c;

			q = g_getterm(p);
			if (present) prline("\n");
			fputs("[   ",f);
			level += 4;
			if (q->t_flags & RESOLVER) {
				prline("%while (..)\n");
			}
			if (q->t_flags & PERSISTENT) {
				prline("%persistent\n");
			}
			if (r_getkind(q) != FIXED) {
				if (!(q->t_flags & PERSISTENT)) {
				    prline("> continue repetition on the\n");
				}
				printset(q->t_first, c_first);
				if (q->t_flags & PERSISTENT) {
				    prline("> continue repetition on the\n");
				}
				printset(q->t_contains, c_contains);
				prline("> terminate repetition on the\n");
				printset(q->t_follow,c_follow);
				if (q->t_flags & EMPTYFIRST) {
				    prline(">>> empty first\n");
				}
				if (q->t_flags & EMPTYTERM) {
				    prline(">>> term produces empty\n");
				}
				cfcheck(q->t_first,q->t_follow,
					q->t_flags & RESOLVER);
			}
			prrule(q->t_rule);
			level -= 4;
			spaces();
			c = r_getkind(q);
			fputs(c == STAR ? "]*" : c == PLUS ? "]+" :
			      c == OPT ? "]?" : "]", f);
			if (c = r_getnum(q)) {
				fprintf(f,"%d",c);
			}
			prline("\n");
			break; }
		  case ACTION :
			fputs("{..} ",f);
			break;
		  case ALTERNATION : {
			register p_link l;

			l = g_getlink(p);
			if (firstalt) {
				firstalt = 0;
			}
			else	prline("|\n");
			printset(l->l_symbs,"> alternative on ");
			cfcheck(l->l_symbs,
				l->l_others,
				(int)(l->l_flag&(COND|PREFERING|AVOIDING)));
			fputs("    ",f);
			level += 4;
			if (l->l_flag & DEF) {
				prline("%default\n");
			}
			if (l->l_flag & AVOIDING) {
				prline("%avoid\n");
			}
			if (l->l_flag & PREFERING) {
				prline("%prefer\n");
			}
			if (l->l_flag & COND) {
				prline("%if ( ... )\n");
			}
			prrule(l->l_rule);
			level -= 4;
			if (g_gettype(p+1) == EORULE) {
				return;
			}
			spaces();
			p++; continue; }
		  case LITERAL :
		  case TERMINAL : {	
			register p_token pt = &tokens[g_getcont(p)];

			fprintf(f,pt->t_tokno<0400 ?
				  "'%s' " : "%s ", pt->t_string);
			break; }
		  case NONTERM :
			fprintf(f,"%s ",nonterms[g_getcont(p)].n_name);
			break;
		}
		p++;
		present = 1;
	}
}

STATIC
cfcheck(s1,s2,flag) p_set s1,s2; {
	/*
	 * Check if s1 and s2 have elements in common.
	 * If so, flag must be non-zero, indicating that there is a
	 * conflict resolver, otherwise, flag must be zero, indicating
	 * that there is not.
	 */
	register p_set temp;

	temp = setalloc();
	setunion(temp,s1);
	if (!setintersect(temp,s2)) {
		if (! flag) {
			printset(temp,">>> conflict on ");
			prline("\n");
		}
	} else {
		if (flag) {
			prline(">>> %if/%while, no conflict\n");
		}
	}
	free((p_mem) temp);
}

STATIC
resolve(p) register p_gram p; {
	/*
	 * resolve conflicts, as specified by the user
	 */
	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			return;
		  case TERM :
			resolve(g_getterm(p)->t_rule);
			break;
		  case ALTERNATION : {
			register p_link	l;

			l = g_getlink(p);
			if (l->l_flag & AVOIDING) {
				/*
				 * On conflicting symbols, this rule
				 * is never chosen
				 */
				setminus(l->l_symbs,l->l_others);
			}
			if (setempty(l->l_symbs)) {
				/*
				 * This may be caused by the statement above
				 */
				error(p->g_lineno,"Alternative never chosen");
			}
			resolve(l->l_rule);
			break; }
		}
		p++;
	}
}

STATIC
propagate(set,p) p_set set; register p_gram p; {
	/*
	 * Propagate the fact that on the elements of set the grammar rule
	 * p will not be chosen.
	 */
	while (g_gettype(p) != EORULE) {
		setminus(g_getlink(p)->l_symbs,set);
		p++;
	} 
}

STATIC
spaces() {

	if (level > 0) fprintf(fout,"%*c",level,' ');
}
