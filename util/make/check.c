/*
 *	Check structures for make.
 *
 *	$Header$
 */

#include <stdio.h>
#include "h.h"


/*
 *	Prints out the structures as defined in memory.  Good for check
 *	that you make file does what you want (and for debugging make).
 */
void
prt()
{
	register struct name *		np;
	register struct depend *	dp;
	register struct line *		lp;
	register struct cmd *		cp;
	register struct macro *		mp;


	for (mp = macrohead; mp; mp = mp->m_next)
		fprintf(stderr, "%s = %s\n", mp->m_name, mp->m_val);

	fputc('\n', stderr);

	for (np = namehead.n_next; np; np = np->n_next)
	{
		if (np->n_flag & N_DOUBLE)
			fprintf(stderr, "%s::\n", np->n_name);
		else
			fprintf(stderr, "%s:\n", np->n_name);
		if (np == firstname)
			fprintf(stderr, "(MAIN NAME)\n");
		for (lp = np->n_line; lp; lp = lp->l_next)
		{
			fputc(':', stderr);
			for (dp = lp->l_dep; dp; dp = dp->d_next)
				fprintf(stderr, " %s", dp->d_name->n_name);
			fputc('\n', stderr);

			for (cp = lp->l_cmd; cp; cp = cp->c_next)
#ifdef os9
				fprintf(stderr, "-   %s\n", cp->c_cmd);
#else
				fprintf(stderr, "-\t%s\n", cp->c_cmd);
#endif
			fputc('\n', stderr);
		}
		fputc('\n', stderr);
	}
}


/*
 *	Recursive routine that does the actual checking.
 */
void
check(np)
struct name *		np;
{
	register struct depend *	dp;
	register struct line *		lp;


	if (np->n_flag & N_MARK)
		fatal("Circular dependency from %s", np->n_name);

	np->n_flag |= N_MARK;

	for (lp = np->n_line; lp; lp = lp->l_next)
		for (dp = lp->l_dep; dp; dp = dp->d_next)
			check(dp->d_name);

	np->n_flag &= ~N_MARK;
}


/*
 *	Look for circular dependancies.
 *	ie.
 *		a: b
 *		b: a
 *	is a circular dep
 */
void
circh()
{
	register struct name *	np;


	for (np = namehead.n_next; np; np = np->n_next)
		check(np);
}


/*
 *	Check the target .PRECIOUS, and mark its dependentd as precious
 */
void
precious()
{
	register struct depend *	dp;
	register struct line *		lp;
	register struct name *		np;


	if (!((np = newname(".PRECIOUS"))->n_flag & N_TARG))
		return;

	for (lp = np->n_line; lp; lp = lp->l_next)
		for (dp = lp->l_dep; dp; dp = dp->d_next)
			dp->d_name->n_flag |= N_PREC;
}
