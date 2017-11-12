#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cgg_cg.h>
#include "data.h"
#include "result.h"
#include "extern.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

move(tp1,tp2,ply,toplevel,maxcost) token_p tp1,tp2; unsigned maxcost; {
	register move_p mp;
	unsigned t;
	register struct reginfo *rp;
	register byte *tdpb;
	int i;

	if (eqtoken(tp1,tp2))
		return(0);
	if (tp2->t_token == -1) {
		if (tp1->t_token == -1) {
			if (eqtoken(&machregs[tp1->t_att[0].ar].r_contents,
				    &machregs[tp2->t_att[0].ar].r_contents) &&
			      machregs[tp1->t_att[0].ar].r_contents.t_token!=0)
				return(0);
		} else {
			if (eqtoken(&machregs[tp2->t_att[0].ar].r_contents,tp1))
				return(0);
		}
		erasereg(tp2->t_att[0].ar);
	} else if (tp1->t_token == -1) {
		if (eqtoken(tp2,&machregs[tp1->t_att[0].ar].r_contents))
			return(0);
	}
	/*
	 * If we arrive here the move must really be executed
	 */
	for (mp=moves;mp->m_set1>=0;mp++) {
		if (!match(tp1,&machsets[mp->m_set1],mp->m_expr1))
			continue;
		if (match(tp2,&machsets[mp->m_set2],mp->m_expr2))
			break;
		/*
		 * Correct move rule is found
		 */
	}
	assert(mp->m_set1>=0);
	/*
	 * To get correct interpretation of things like %[1]
	 * in move code we stack tp2 and tp1. This little trick
	 * saves a lot of testing in other places.
	 */

	fakestack[stackheight] = *tp2;
	fakestack[stackheight+1] = *tp1;
	stackheight += 2;
	tokpatlen += 2;
	t = codegen(&coderules[mp->m_cindex],ply,toplevel,maxcost,0);
	tokpatlen -= 2;
	stackheight -= 2;
	if (tp2->t_token == -1) {
		rp = &machregs[tp2->t_att[0].ar];
		if (tp1->t_token == -1) {
			rp->r_contents =
			  machregs[tp1->t_att[0].ar].r_contents ;
		}
		else	rp->r_contents = *tp1;
		if (rp->r_contents.t_token > 0) {
			tdpb = &(tokens[rp->r_contents.t_token].t_type[0]);
			for (i=0;i<TOKENSIZE;i++)
				if (*tdpb++ == EV_REG &&
				    clash(rp->r_contents.t_att[i].ar,tp2->t_att[0].ar)) {
					rp->r_contents.t_token = 0;
					for (i = 0; i < TOKENSIZE; i++)
						rp->r_contents.t_att[i].aw = 0;
					break;
				}
		}
	}
	else if (tp1->t_token == -1)
		machregs[tp1->t_att[0].ar].r_contents = *tp2;
	return(t);
}

#define cocoreg machregs[0].r_contents

setcc(tp) token_p tp; {

	cocoreg = *tp;
}

test(tp,ply,toplevel,maxcost) token_p tp; unsigned maxcost; {
	register test_p mp;
	unsigned t;

	if (cocoreg.t_token!=0) {
		if (eqtoken(tp,&cocoreg))
			return(0);
		if (tp->t_token == -1) {
			if (eqtoken(&machregs[tp->t_att[0].ar].r_contents,&cocoreg))
				return(0);
		}
	}
	/*
	 * If we arrive here the test must really be executed
	 */
	for (mp=tests;mp->t_set>=0;mp++) {
		if (match(tp,&machsets[mp->t_set],mp->t_expr))
			break;
		/*
		 * Correct move rule is found
		 */
	}
	assert(mp->t_set>=0);
	/*
	 * To get correct interpretation of things like %[1]
	 * in test code we stack tp. This little trick
	 * saves a lot of testing in other places.
	 */

	fakestack[stackheight] = *tp;
	stackheight++;
	tokpatlen++;
	t = codegen(&coderules[mp->t_cindex],ply,toplevel,maxcost,0);
	tokpatlen--;
	stackheight--;
	return(t);
}
