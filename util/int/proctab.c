/** @file
	proctable management routines.
*/

/* $Id$ */

#include	"logging.h"
#include	"global.h"
#include	"log.h"
#include	"io.h"
#include	"alloc.h"
#include	"proctab.h"

/** Procedure table */
struct proc *proctab;
PRIVATE long pr_cnt;

/** Allocates and initializes the procedure table. */
void init_proctab(void)
{
	proctab = (struct proc *)
			Malloc(NProc * sizeof (struct proc), "proctable");
	pr_cnt = 0;
}

/** Add a procedure to the procedure entry table.
 *  "ep" is the pointer to the entry point of the
 *  procedure to add.
 */
void add_proc(size nloc, ptr ep)
{
	register struct proc *pr = &proctab[pr_cnt++];
	register struct proc *p;
	register ptr ff = DB;

	LOG((" r6 add_proc: pr_cnt = %ld, nloc = %lu, ep = %lu",
				pr_cnt-1, nloc, ep));
	if (ep > DB)
		fatal("procedure entry point outside text segment");

	pr->pr_nloc = nloc;
	pr->pr_ep = ep;
	/* examine all old proc descriptors */
	for (p = &proctab[0]; p < pr; p++) {
		if (	/* the old one starts earlier */
			p->pr_ep < pr->pr_ep
		&&	/* it seems to end later */
			p->pr_ff > pr->pr_ep
		) {	/* update its limit */
			p->pr_ff = pr->pr_ep;
		}
		if (	/* the old one starts later */
			p->pr_ep > pr->pr_ep
		&&	/* our limit is beyond the old procedure entry point*/
			ff > p->pr_ep
		) {	/* update our limit */
			ff = p->pr_ep;
		}
	}
	pr->pr_ff = ff;
}

void end_init_proctab(void)
{
#ifdef	LOGGING
	register long p;

	if (!check_log(" r6"))
		return;

	for (p = 0; p < NProc; p++) {
		register struct proc *pr = &proctab[p];

		LOG((" r5: proctab[%ld]: nloc = %d, ep = %lu, ff = %lu",
				p, pr->pr_nloc, pr->pr_ep, pr->pr_ff));
	}
#endif	/* LOGGING */
}

