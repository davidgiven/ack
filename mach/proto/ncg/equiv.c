#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "assert.h"
#include "equiv.h"
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cgg_cg.h>
#include "data.h"
#include "result.h"
#include "extern.h"

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
 * Author: Hans van Staveren
 */

extern string myalloc();

int rar[MAXCREG];
rl_p *lar;
int maxindex;
int regclass[NREGS];
struct perm *perms;

struct perm *
tuples(regls,nregneeded) rl_p *regls; {
	int class=0;
	register i,j;

	/*
	 * First compute equivalence classes of registers.
	 */

	for (i=0;i<NREGS;i++) {
		regclass[i] = class++;
		if (getrefcount(i) == 0) {
			for (j=0;j<i;j++) {
				if (eqregclass(i,j) &&
				    eqtoken(&machregs[i].r_contents,
					    &machregs[j].r_contents)) {
					regclass[i] = regclass[j];
					break;
				}
			}
		}
	}

	/*
	 * Now create tuples through a recursive function
	 */

	maxindex = nregneeded;
	lar = regls;
	perms = 0;
	permute(0);
	return(perms);
}

permute(index) {
	register struct perm *pp;
	register rl_p rlp;
	register i,j;

	if (index == maxindex) {
		for (pp=perms; pp != 0; pp=pp->p_next) {
			for (i=0; i<maxindex; i++)
				if (regclass[rar[i]] != regclass[pp->p_rar[i]])
					goto diff;
			for (i=0; i<maxindex; i++)
				for (j=0; j<i; j++)
					if (clash(rar[i],rar[j]) !=
					    clash(pp->p_rar[i],pp->p_rar[j]))
						goto diff;
			return;
		    diff: ;
		}
		pp = (struct perm *) myalloc(sizeof ( *pp ));
		pp->p_next = perms;
		for (i=0; i<maxindex; i++)
			pp->p_rar[i] = rar[i];
		perms = pp;
	} else {
		rlp=lar[index];
		for (i=rlp->rl_n-1; i>=0; i--) {
			rar[index] = rlp->rl_list[i];
			permute(index+1);
		}
	}
}
