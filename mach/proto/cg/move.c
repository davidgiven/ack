/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include "assert.h"
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cg_pattern.h>
#include "data.h"
#include "result.h"
#include "extern.h"
#include "subr.h"
#include "reg.h"
#include "codegen.h"


int move(token_p tp1,token_p tp2,int ply,int toplevel,unsigned int maxcost)
{
	register move_p mp;
	register unsigned t;
	register struct reginfo *rp;
	tkdef_p tdp;
	int i;

	if (eqtoken(tp1,tp2))
		return(0);
	if (tp2->t_token == -1) {
		if (tp1->t_token == -1) {
			if (eqtoken(&machregs[tp1->t_att[0].ar].r_contents,
				    &machregs[tp2->t_att[0].ar].r_contents) &&
			      machregs[tp1->t_att[0].ar].r_contents.t_token!=0)
				return(0);
			if (tp1->t_att[0].ar!=1) { /* COCO reg; tmp kludge */
				erasereg(tp2->t_att[0].ar);
				machregs[tp2->t_att[0].ar].r_contents =
				  machregs[tp1->t_att[0].ar].r_contents ;
			} else
				machregs[tp1->t_att[0].ar].r_contents =
				  machregs[tp2->t_att[0].ar].r_contents ;
		} else {
			if (eqtoken(&machregs[tp2->t_att[0].ar].r_contents,tp1))
				return(0);
			erasereg(tp2->t_att[0].ar);
			machregs[tp2->t_att[0].ar].r_contents = *tp1;
		}
		for (rp=machregs;rp<machregs+NREGS;rp++) {
			if (rp->r_contents.t_token == 0)
				continue;
			assert(rp->r_contents.t_token > 0);
			tdp = &tokens[rp->r_contents.t_token];
			for (i=0;i<TOKENSIZE;i++)
				if (tdp->t_type[i] == EV_REG &&
				    clash(rp->r_contents.t_att[i].ar,tp2->t_att[0].ar)) {
					erasereg((int)(rp-machregs));
					break;
				}
		}
	} else if (tp1->t_token == -1) {
		if (eqtoken(tp2,&machregs[tp1->t_att[0].ar].r_contents))
			return(0);
		machregs[tp1->t_att[0].ar].r_contents = *tp2;
	}
	/*
	 * If we arrive here the move must really be executed
	 */
	for (mp=moves;mp<moves+NMOVES;mp++) {
		if (!match(tp1,&machsets[mp->m_set1],mp->m_expr1))
			continue;
		if (match(tp2,&machsets[mp->m_set2],mp->m_expr2))
			break;
		/*
		 * Correct move rule is found
		 */
	}
	assert(mp<moves+NMOVES);
	/*
	 * To get correct interpretation of things like %[1]
	 * in move code we stack tp2 and tp1. This little trick
	 * saves a lot of testing in other places.
	 */

	if (mp->m_cindex!=0) {
		fakestack[stackheight] = *tp2;
		fakestack[stackheight+1] = *tp1;
		stackheight += 2;
		t = codegen(&coderules[mp->m_cindex],ply,toplevel,maxcost,0);
		if (t <= maxcost)
			t += costcalc(mp->m_cost);
		stackheight -= 2;
	} else {
		t = 0;
	}
	return(t);
}
