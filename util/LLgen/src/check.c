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
 * check.c
 * Several routines to perform checks and printouts
 */

# include "types.h"
# include "extern.h"
# include "tunable.h"
# include "io.h"
# include "sets.h"
# include "assert.h"

# ifndef NORCSID
static string rcsid1 = "$Header$";
# endif

# define PTERM(p) fprintf(fout,(p)->h_num < 0400 ? "'%s' " : "%s ",(p)->h_name);

static string	c_first = "> firstset   ";
static string	c_contains = "> containset ";
static string	c_follow = "> followset  ";
p_set		setalloc();
static string	ntname;		/* nonterminal that is currently checked */
static p_nont	nt;		/* pointer to its struct */
static int	level;

/* In this file are defined : */
extern conflchecks();
STATIC newline();
STATIC printset();
STATIC check();
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
	register FILE	*f;
	register short	*s;
	p_file		x = files;

	f_input = x->f_name;
	if (verbose >= 3) {
		for (p = nonterms; p < maxnt; p++) p->n_flags |= VERBOSE;
	}
	if (verbose) {
		if ((f = fopen(f_out,"w")) == NULL) fatal(1,e_noopen,f_out);
		fout = f;
	}
	/*
	 * Check the rules in the order in which they are declared,
	 * and input file by input file, to give proper error messages
	 */
	for (; x->f_end < maxorder; x++) {
	    f_input = x->f_name;
# ifndef NDEBUG
	if (debug) fprintf(stderr, "CHECK: file %s\n", f_input);
# endif
	    for (s = x->f_start; s <= x->f_end; s++) {
		nt = p = &nonterms[*s];
		ntname = (min_nt_ent + *s)->h_name;
# ifndef NDEBUG
		if (debug) fprintf(stderr,"CHECK: nonterminal %s\n", ntname);
# endif
		if (p->n_flags & RECURSIVE) {
			error(p->n_lineno,
				"Recursion in default for nonterminal %s",
				ntname);
		}
		check(p->n_rule);
		/*
		 * If a printout is needed for this rule in
		 * LL.output, just do it
		 */
		if (verbose && (p->n_flags & VERBOSE)) {
			fprintf(f,"\n%s :\n",ntname);
			printset(p->n_first,c_first);
			printset(p->n_contains,c_contains);
			printset(p->n_follow,c_follow);
			fputs("> rule\n\t",f);
			level = 8;
			prrule(p->n_rule);
			level = 0;
			newline();
		}
		/*
		 * Now, the conflicts may be resolved
		 */
		resolve(p->n_rule);
	    }
	}
	if (verbose) fclose(f);
}

STATIC
newline() {
	/*
	 * Newline and "level" spaces indentation
	 */
	if (level > 0) fprintf(fout,"\n%*c",level,' ');
	else putc('\n',fout);
}

STATIC
printset(p,s) register p_set p; string s; {
	/*
	 * Print the elements of a set
	 */
	register FILE	*f;
	register	i;
	register	j;
	int		k;
	int		hulp;

	k = strlen(s)+1;
	/*
	 * k contains relative level of indentation
	 */
	f = fout;
	fprintf(f,"%s{ ",s);
	j = level+1+k;
	/*
	 * j will gather the total length of the line
	 */
	if (p == (p_set) 0) fputs(">non existent (yet?)< ",f);
	else {
		for (i = 0; i < nterminals; i++) {
			if (IN(p,i)) {
				hulp = strlen(h_entry[i].h_name)+1;
				if (h_entry[i].h_num < 0400) hulp += 2;
				if ((j += hulp) >= 78) {
					/*
					 * Line becoming too long
					 */
					j = level+k+1+hulp;
					newline();
					fprintf(f,">%*c",k,' ');
				}
				PTERM(&h_entry[i]);
			}
		}
		if (ntprint) for (i = 0; i < nnonterms; i++) {
			/*
			 * Nonterminals in the set must also be printed
			 */
			if (NTIN(p,i)) {
				hulp = strlen((min_nt_ent+i)->h_name)+3;
				if ((j += hulp) >= 78) {
					j = level + k + 1 + hulp;
					newline();
					fprintf(f,">%*c",k,' ');
				}
				fprintf(f,"<%s> ",(min_nt_ent+i)->h_name);
			}
		}
	}
	putc('}',f);
	newline();
}

STATIC
check(p) register p_gram p; {
	/*
	 * Search for conflicts in a grammar rule.
	 */
	p_set	temp;

	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			return;
		  case NONTERM : {
			register p_nont n;

			n = &nonterms[g_getnont(p)];
			if (g_getnpar(p) != getntparams(n)) {
			    error(p->g_lineno,
			        "Call of %s : parameter count mismatch",
				(min_nt_ent+g_getnont(p))->h_name);
			}
			break; }
		  case TERM : {
			register p_term q;

			q = (p_term) pentry[g_getcont(p)];
			check(q->t_rule);
			if (r_getkind(&(q->t_reps)) == FIXED) break;
			if (setempty(q->t_first)) {
				q->t_flags |= EMPTYFIRST;
				nt->n_flags |= VERBOSE;
				error(p->g_lineno, "No symbols in term");
			}
			if (empty(q->t_rule)) {
				q->t_flags |= EMPTYTERM;
				nt->n_flags |= VERBOSE;
				error(p->g_lineno, "Term produces empty");
			}
			temp = setalloc(setsize);
			setunion(temp,q->t_first,setsize);
			if (!setintersect(temp,q->t_follow,setsize)) {
				/* 
			 	 * q->t_first * q->t_follow != EMPTY
			 	 */
				if (!(q->t_flags & RESOLVER)) {
			  		/*
			   		 * No conflict resolver
			   		 */
					error(p->g_lineno,
						"Repitition conflict");
					nt->n_flags |= VERBOSE;
					if (verbose == 2) moreverbose(temp);
				}
			} else {
				if (q->t_flags & RESOLVER) {
					q->t_flags |= NOCONF;
					error(p->g_lineno,
						"%%while, no conflict");
					nt->n_flags |= VERBOSE;
				}
			}
			free((p_mem) temp);
			break; }
		  case ALTERNATION : {
			register p_link l;

			l = (p_link) pentry[g_getcont(p)];
			temp = setalloc(setsize);
			setunion(temp,l->l_symbs,setsize);
			if(!setintersect(temp,l->l_others,setsize)) {
				/*
				 * temp now contains the conflicting
				 * symbols
				 */
				if (!(l->l_flag & (COND|PREFERING|AVOIDING))) {
					error(p->g_lineno,
						"Alternation conflict");
					nt->n_flags |= VERBOSE;
					if (verbose == 2) moreverbose(temp);
				} 
			} else {
				if (l->l_flag & (COND|PREFERING|AVOIDING)) {
					l->l_flag |= NOCONF;
					error(p->g_lineno,"No conflict");
					nt->n_flags |= VERBOSE;
				}
			}
			free( (p_mem) temp);
			check(l->l_rule);
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
	register i;

	if (verbose) for (i = 0; i < nnonterms; i++) {
		if (NTIN(t,i)) nonterms[i].n_flags |= VERBOSE;
	}
}

STATIC
prrule(p) register p_gram p; {
	/*
	 * Create a verbose printout of grammar rule p
	 */
	register FILE	*f;
	int		present = 0;

	f = fout;
	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			putc('\n',f);
			return;
		  case TERM : {
			register p_term	q;
			register	c;

			q = (p_term) pentry[g_getcont(p)];
			if (present) newline();
			if (r_getkind(&(q->t_reps)) != FIXED ||
			    r_getnum(&(q->t_reps)) != 0) {
				fputs("[   ",f);
				level += 4;
				if (q->t_flags & RESOLVER) {
					fputs("%%while (..)",f);
					newline();
				}
				if (r_getkind(&(q->t_reps)) != FIXED) {
			    	    printset(q->t_first, c_first);
				    printset(q->t_contains, c_contains);
				    printset(q->t_follow,c_follow);
				    if (q->t_flags & EMPTYFIRST) {
					fputs(">>> empty first",f);
					newline();
				    }
				    if (q->t_flags & EMPTYTERM) {
					fputs(">>> term produces empty",f);
					newline();
				    }
				    cfcheck(q->t_first,q->t_follow,
						q->t_flags & RESOLVER);
				}
				prrule(q->t_rule);
				level -= 4;
				spaces();
				c = r_getkind(&(q->t_reps));
				putc(']',f);
				if (c != FIXED) {
					c = (c==STAR)?'*':(c==PLUS)?'+':'?';
					putc(c,f);
				}
				if (c = r_getnum(&(q->t_reps))) {
					fprintf(f,"%d",c);
				}
				newline();
			} else {
				prrule(q->t_rule);
				spaces();
			}
			break; }
		  case ACTION :
			fputs("{..} ",f);
			break;
		  case ALTERNATION : {
			register p_link l;

			l = (p_link) pentry[g_getcont(p)];
			if (l->l_flag & (COND|PREFERING|AVOIDING)) {
				printset(l->l_symbs,"> alt with resolver on ");
			} else	printset(l->l_symbs,"> alternative on ");
			cfcheck(l->l_symbs,
				l->l_others,
				(int)(l->l_flag&(COND|PREFERING|AVOIDING)));
			fputs("    ",f);
			level += 4;
			prrule(l->l_rule);
			level -= 4;
			spaces();
			if (g_gettype(p+1) == EORULE) {
				fputs("> end alternatives\n",f);
				return;
			}
			p++; continue; }
		  case TERMINAL :
			PTERM(&h_entry[g_getcont(p)]);
			break;
		  case NONTERM :
			fprintf(f,"%s ",(g_getnont(p)+min_nt_ent)->h_name);
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

	temp = setalloc(setsize);
	setunion(temp,s1,setsize);
	if (!setintersect(temp,s2,setsize)) {
		if (! flag) {
			printset(temp,">>> conflict on ");
			newline();
		}
	} else {
		if (flag) {
			fputs(">>> %if/%while, no conflict",fout);
			newline();
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
			resolve(((p_term) pentry[g_getcont(p)])->t_rule);
			break;
		  case ALTERNATION : {
			register p_link	l;

			l = (p_link) pentry[g_getcont(p)];
			if (l->l_flag & AVOIDING) {
				/*
				 * On conflicting symbols, this rule
				 * is never chosen
				 */
				setminus(l->l_symbs,l->l_others,setsize);
			}
			if (setempty(l->l_symbs)) {
				/*
				 * This may be caused by the statement above
				 */
				error(p->g_lineno,"Alternative never chosen");
			}
			resolve(l->l_rule);
			if (l->l_flag & PREFERING) propagate(l->l_symbs,p+1);
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
		setminus(((p_link) pentry[g_getcont(p)])->l_symbs,set,setsize);
		p++;
	} 
}

STATIC
spaces() {

	if (level > 0) fprintf(fout,"%*c",level,' ');
}
