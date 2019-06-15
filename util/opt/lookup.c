/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include <stdlib.h>
#include <string.h>
#include "param.h"
#include "types.h"
#include "tes.h"
#include "lookup.h"
#include "alloc.h"
#include "proinf.h"
#include "util.h"

unsigned hash(char *string)
{
	register char *p;
	register unsigned i, sum;

	for (sum = i = 0, p = string; *p; i += 3)
		sum ^= (*p++) << (i & 07);
	return (sum);
}

sym_p symlookup(char *name, int status, int flags)
{
	register sym_p *spp, sp;
	register int i;
	static short genfrag = 32767;

	spp = &symhash[hash(name) % NSYMHASH];
	while (*spp != (sym_p) 0)
		if (strncmp((*spp)->s_name, name, IDL) == 0)
		{
			sp = *spp;
			if ((sp->s_flags ^ flags) & SYMPRO)
				error("%s is both proc and datalabel", name);
			if (status == DEFINING)
			{
				if (sp->s_flags & SYMDEF)
					error("redefined symbol %s", name);
				sp->s_flags |= SYMDEF;
			}
			return (sp);
		}
		else
			spp = &(*spp)->s_next;

	/*
	 * symbol not found, enter in table
	 */

	i = strlen(name) + 1;
	if (i & 1)
		i++;
	if (i > IDL)
		i = IDL;
	*spp = sp = newsym(i);
	strncpy(sp->s_name, name, i);
	sp->s_flags = flags;
	if (status == DEFINING)
		sp->s_flags |= SYMDEF;
	sp->s_frag = genfrag--;
	return (sp);
}

num_p numlookup(unsigned number)
{
	register num_p *npp, np;

	npp = &curpro.numhash[number % NNUMHASH];
	while (*npp != (num_p) 0)
		if ((*npp)->n_number == number)
			return (*npp);
		else
			npp = &(*npp)->n_next;

	/*
	 * local label not found, enter in tabel
	 */

	*npp = np = newnum();
	np->n_number = number;
	np->n_repl = np;
	return (np);
}
