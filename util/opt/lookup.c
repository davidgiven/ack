#include "param.h"
#include "types.h"
#include "lookup.h"
#include "alloc.h"
#include "proinf.h"

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

unsigned hash(string) char *string; {
	register char *p;
	register unsigned i,sum;

	for (sum=i=0,p=string;*p;i += 3)
		sum ^= (*p++)<<(i&07);
	return(sum);
}

sym_p symlookup(name,status,flags) char *name; int status,flags; {
	register sym_p *spp,sp;
	static short genfrag = 32767;

	spp = &symhash[hash(name)%NSYMHASH];
	while (*spp != (sym_p) 0)
		if (strncmp((*spp)->s_name,name,IDL)==0) {
			sp = *spp;
			if ((sp->s_flags^flags)&SYMPRO)
				error("%s is both proc and datalabel",name);
			if (status == DEFINING) {
				if (sp->s_flags&SYMDEF)
					error("redefined symbol %s",name);
				sp->s_flags |= SYMDEF;
			}
			return(sp);
		} else
			spp = &(*spp)->s_next;

	/*
	 * symbol not found, enter in table
	 */

	*spp = sp = newsym();
	strncpy(sp->s_name,name,IDL);
	sp->s_flags = flags;
	if (status == DEFINING)
		sp->s_flags |= SYMDEF;
	sp->s_frag = genfrag--;
	return(sp);
}

num_p numlookup(number) unsigned number; {
	register num_p *npp, np;

	npp = &curpro.numhash[number%NNUMHASH];
	while (*npp != (num_p) 0)
		if ((*npp)->n_number == number)
			return(*npp);
		else
			npp = &(*npp)->n_next;

	/*
	 * local label not found, enter in tabel
	 */

	*npp = np = newnum();
	np->n_number = number;
	np->n_repl = np;
	return(np);
}
