/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include "assert.h"
#include "param.h"
#include "set.h"
#include <stdio.h>

/*
 * This file implements the marriage thesis from Hall.
 * The thesis says that given a number, say N, of subsets from
 * a finite set, it is possible to create a set with cardinality N,
 * that contains one member for each of the subsets,
 * iff for each number, say M, of subsets from 2 to N the union of
 * each M-tuple sets has cardinality >= M.
 *
 * So what, you might say. As indeed I did.
 * But this is actually used here to check the possibility of each
 * code rule. If a code rule has a number of token_sets in the with
 * clause and a number of properties in the uses rule it must be
 * possible to do this from an empty fakestack. Hall helps.
 */

#define MAXHALL (TOKPATMAX+MAXALLREG)
short hallsets[MAXHALL][SETSIZE];
int nhallsets= -1;
int hallfreq[MAXHALL][2];

hallverbose() {
	register i;
	register max;
	
	fprintf(stderr,"Table of hall frequencies\n   #   pre   post\n");
	for (max=MAXHALL-1;hallfreq[max][0]==0 && hallfreq[max][1]==0;max--)
		;
	for (i=0;i<=max;i++)
		fprintf(stderr,"%3d%6d%6d\n",i,hallfreq[i][0],hallfreq[i][1]);
}

inithall() {

	assert(nhallsets == -1);
	nhallsets=0;
}

nexthall(sp) register short *sp; {
	register i;
	
	assert(nhallsets>=0);
	for(i=0;i<SETSIZE;i++)
		hallsets[nhallsets][i] = sp[i];
	nhallsets++;
}

card(sp) register short *sp; {
	register sum,i;
	
	sum=0;
	for(i=0;i<8*sizeof(short)*SETSIZE;i++)
		if (BIT(sp,i))
			sum++;
	return(sum);
}

checkhall() {

	assert(nhallsets>=0);
	if (!hall())
		error("Hall says: \"You can't have those registers\"");
}

hall() {
	register i,j,k;
	int ok;
	
	hallfreq[nhallsets][0]++;
	/*
	 * If a set has cardinality >= nhallsets it can never be the cause
	 * of the hall algorithm failing. So it can be thrown away.
	 * But then nhallsets is less, so this step can be re-applied.
	 */

	do {
		ok = 0;
		for(i=0;i<nhallsets;i++)
			if (card(hallsets[i])>=nhallsets) {
				for (j=i+1;j<nhallsets;j++)
					for(k=0;k<SETSIZE;k++)
						hallsets[j-1][k] =
							hallsets[j][k];
				nhallsets--;
				ok = 1;
				break;
			}
	} while (ok);
	
	/*
	 * Now all sets have cardinality < nhallsets
	 */
	
	hallfreq[nhallsets][1]++;
	ok=recurhall(nhallsets,hallsets);
	nhallsets = -1;
	return(ok);
}

recurhall(nhallsets,hallsets) short hallsets[][SETSIZE]; {
	short copysets[MAXHALL][SETSIZE];
	short setsum[SETSIZE];
	register i,j,k,ncopys;
	
	/*
	 * First check cardinality of union of all
	 */
	for(k=0;k<SETSIZE;k++)
		setsum[k]=0;
	for(i=0;i<nhallsets;i++)
		unite(hallsets[i],setsum);
	if (card(setsum)<nhallsets)
		return(0);
	/*
	 * Now check the hall property of everything but one set,
	 * for all sets
	 */
	for(i=0;i<nhallsets;i++) {
		ncopys=0;
		for(j=0;j<nhallsets;j++) if (j!=i) {
			for(k=0;k<SETSIZE;k++)
				copysets[ncopys][k] = hallsets[j][k];
			ncopys++;
		}
		assert(ncopys == nhallsets-1);
		if (!recurhall(ncopys,copysets))
			return(0);
	}
	return(1);
}

unite(sp,into) register short *sp,*into; {
	register i;
	
	for(i=0;i<SETSIZE;i++)
		into[i] |= sp[i];
}

/*
 * Limerick (rot13)
 *
 * N zngurzngvpvna anzrq Unyy
 * Unf n urknurqebavpny onyy,
 * 	Naq gur phor bs vgf jrvtug
 * 	Gvzrf uvf crpxre'f, cyhf rvtug
 * Vf uvf cubar ahzore -- tvir uvz n pnyy..
 */
