/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* C O M M O N   S U B E X P R E S S I O N   E L I M I N A T I O N */


#include <stdlib.h>
#include <stdio.h>
#include "../share/types.h"
#include "../share/lset.h"
#include "../share/debug.h"
#include "../share/go.h"
#include "cs.h"
#include "cs_aux.h"
#include "cs_avail.h"
#include "cs_debug.h"
#include "cs_elim.h"
#include "cs_entity.h"
#include "cs_profit.h"
#include "cs_stack.h"
#include "cs_vnm.h"

int Scs; /* Number of optimizations found. */

STATIC cs_clear()
{
	clr_avails();
	clr_entities();
	clr_stack();

	start_valnum();
}

STATIC void cs_optimize(p)
	proc_p p;
{
	/* Optimize all basic blocks of one procedure. */

	register bblock_p rbp, bdone;

	if (IS_ENTERED_WITH_GTO(p)) return;
	avails = (avail_p) 0;
	entities = Lempty_set();
	cs_clear();

	rbp = p->p_start;

	while (rbp != (bblock_p) 0) {
		/* First we build a list of common expressions with the
		 * value numbering algorithm. We take blocks in textual order
		 * as long as the next block can only be reached through the
		 * block we have just done. Note that if a block is preceded
		 * by itself, the number of predecessors is greater than 1,
		 * but the previous block can still be its immediate dominator.
		 */
		do {	vnm(rbp); bdone = rbp;
			OUTTRACE("basic block %d processed", bdone->b_id);
			rbp = rbp->b_next;
		} while (rbp != (bblock_p) 0 && rbp->b_idom == bdone &&
			Lnrelems(rbp->b_pred) == 1
		);
		OUTTRACE("value numbering completed", 0);
		OUTAVAILS(); OUTENTITIES();

		/* Now we put out the instructions without common
		 * subexpressions but with the use of temporaries,
		 * which will be local variables of procedure p.
		 */
		eliminate(p);
		cs_clear();
	}
}

main(argc, argv)
	int	argc;
	char	*argv[];
{
	Scs = 0;
	go(argc, argv, no_action, cs_optimize, cs_machinit, no_action);
	report("Duplicate expressions eliminated", Scs);
	exit(0);
}
