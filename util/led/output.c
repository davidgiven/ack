#ifndef lint
static char rcsid[] = "$Header$";
#endif

#include <out.h>
#include "const.h"
#include "memory.h"

static			generate_section_names();

extern struct outhead	outhead;
extern bool		incore;
extern int		flagword;

/*
 * We have counted all relocation structs but we know only now if
 * these must be emitted.We add all names here,unless the -s(trip)
 * flag was given.
 * If this flag is given we don't need the string table either.
 */
beginoutput()
{
	extern ushort	NLocals, NGlobals;
	extern long	NLChars, NGChars;

	if (incore)
		generate_section_names();

	if (!(flagword & RFLAG))
		outhead.oh_nrelo = (ushort)0;
	if (flagword & SFLAG) {
		outhead.oh_nname = (ushort)0;
		outhead.oh_nchar = (long)0;
	} else {
		outhead.oh_nname = NLocals + NGlobals + outhead.oh_nsect;
		outhead.oh_nchar = NLChars + NGChars;
	}
	if (!incore)
		begin_write();
}

/*
 * Generate names for all sections and put them after the global names.
 * Section names are used for relocation.
 */
static
generate_section_names()
{
	register struct outname	*name;
	register int		sectindex;
	register long		size;
	extern struct outsect	outsect[];
	extern char		*core_alloc();

	size = (long)outhead.oh_nsect * sizeof(struct outname); 
	name = (struct outname *)core_alloc(ALLOGLOB, size);
	if (name == (struct outname *)0)
		return;

	for (sectindex = 0; sectindex < outhead.oh_nsect; sectindex++, name++) {
		name->on_foff = (long)0;	/* No string name. */
		name->on_type = (S_MIN + sectindex) | S_SCT;
		name->on_desc = (ushort)0;
		name->on_valu = outsect[sectindex].os_base;
	}
}

/*
 * If we didn't keep the whole output file in core, most of it has been
 * written out, and we just finish that.
 * If we did, we write out our pieces of core.
 */
endoutput()
{
	if (!incore) {
		if (!(flagword & SFLAG))
			end_write();
	} else {
		write_bytes();
	}
}
