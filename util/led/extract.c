#ifndef lint
static char rcsid[] = "$Header$";
#endif

#include "../../h/out.h"
#include "const.h"
#include "debug.h"
#include "defs.h"
#include "memory.h"
#include "orig.h"
#include "scan.h"

static		get_name();
static		process();
static		getexternal();
static		redefine();
static		transfer();

/*
 * Get section sizes and symboltable information from present module.
 */
extract()
{
	struct outhead	head;

	get_modul();
	/*
	 * Copy head because we need it so often but it can change place,
	 * so we can't trust a pointer to it.
	 */
	head = *(struct outhead *)modulptr(IND_HEAD);
	get_names(&head);
	process(&head);
	skip_modul(&head);
}

ushort	NLocals = 0;	/* Number of local names to be saved. */
ushort	NGlobals = 0;	/* Number of global names. */

/*
 * Walk through the nametable of this module, counting the locals that must
 * appear in the final output file if this module is linked.
 * That number will be returned.
 */
static
get_names(head)
	register struct outhead	*head;
{
	register int	nnames;
	register ind_t	sectindex, nameindex, charindex;
	register ind_t	charoff;
	extern int	flagword;

	nnames = head->oh_nname;
	sectindex = IND_SECT(*head);
	nameindex = IND_NAME(*head);
	charindex = IND_CHAR(*head);
	charoff = OFF_CHAR(*head);
	while (nnames--) {
		register struct outsect	*sects;
		struct outname		name;	/* A local copy. */
		/*
		 * Because savelocal/getexternal might relocate the modules
		 * we have to compute the core addresses again.
		 */
		sects = (struct outsect *)modulptr(sectindex);
		name = *(struct outname *)modulptr(nameindex);
		/*
		 * Change the offset in file into an offset in the memory area.
		 * There will always be at least a header before the string
		 * area, so we don't have to be afraid to confuse "no name"
		 * with "the first name".
		 */
		if (name.on_foff)
			name.on_foff += charindex - charoff;
		namerelocate(&name, sects);
		if (name.on_type & S_EXT) {
			getexternal(&name);
		} else {
			/*
			 * The only thing we want to know about locals is
			 * whether they must appear in the output file.
			 */
			if (!(flagword & SFLAG) && mustsavelocal(&name)) {
				NLocals++;
				savelocal(&name);
			}
		}
		nameindex += sizeof(struct outname);
	}
}

extern struct orig	relorig[];

static
process(head)
	register struct outhead	*head;
{
	register struct outsect	*sects;
	register struct outsect	*outsp;
	register int		nsect;
	register struct orig	*orig = relorig;
	extern struct outhead	outhead;
	extern struct outsect	outsect[];

	outhead.oh_nrelo += head->oh_nrelo;
	outhead.oh_nemit += head->oh_nemit;
	if (head->oh_nsect > outhead.oh_nsect)
		outhead.oh_nsect = head->oh_nsect;
	sects = (struct outsect *)modulptr(IND_SECT(*head));
	nsect = head->oh_nsect;
	outsp = outsect;
	while (nsect--) {
		outsp->os_size += sects->os_size;
		outsp->os_flen += sects->os_flen;
		/*
		 * Add all flen's and all (size - flen == zero)'s of
		 * preceding sections with the same number.
		 */
		orig->org_flen += sects->os_flen;
		orig->org_zero += sects->os_size - sects->os_flen;
		orig++; outsp++; sects++;
	}
}

/*
 * Add relocation constant for names in user defined sections.
 * The value of a common name indicates a size instead of an offset,
 * and hence shouldn't be relocated.
 * The value of a name in the zero part of a section is relative from the
 * beginning of the section, not from the beginning of the zero part; but
 * all zero parts will be put after the normal section contents, so we
 * must subtract the flen of its section from the value (and later on add
 * the total flen of its section) and add the accumulated size of all
 * zero parts in preceding sections with the same number.
 * Otherwise we just add the accumulated size of all normal parts in preceding
 * sections with the same size.
 */
namerelocate(name, sects)
	register struct outname	*name;
	struct outsect		*sects;
{
	register int	type = name->on_type;
	register int	sectindex;

	if ((type & S_TYP) == S_UND || (type & S_TYP) == S_ABS)
		return;
	if (type & S_COM) {
		if ( ! (type&S_EXT) ) fatal("local commons should be handled by the assembler") ;
		return;
	}

	sectindex = (type & S_TYP) - S_MIN;
	if (name->on_valu >= sects[sectindex].os_flen) {
		name->on_type |= S_ZER;
		name->on_valu -= sects[sectindex].os_flen;
		name->on_valu += relorig[sectindex].org_zero;
	} else {
		name->on_valu += relorig[sectindex].org_flen;
	}
}

/*
 * If we see this name for the first time, we must remember it for
 * we might need it later on. Otherwise it must confirm to what we already
 * know about it, and eventually add to that knowledge.
 */
static
getexternal(name)
	register struct outname	*name;
{
	register char		*string;
	register int		h;
	register struct outname	*old;
	extern int		hash();
	extern struct outname	*searchname();

	string = modulptr((ind_t)name->on_foff);
	h = hash(string);
	old = searchname(string, h);
	if (old == (struct outname *)0) {
		NGlobals++;
		entername(name, h);
	} else if (!ISUNDEFINED(name)) {
		if (ISUNDEFINED(old)) {
			name->on_mptr = string;	/* Just for convenience. */
			transfer(name, old);
		} else {
			name->on_mptr = string;	/* Just for convenience. */
			redefine(name, old);
		}
	}
}

/*
 * Handle the redefinition of `new' in the current module.
 * A name can be defined in three ways, in increasing priority:
 *	undefined,
 *	common,
 *	defined in a section.
 * A name may become "higher" when defined, but not "lower".
 * A redefinition as common is allowed. It is ignored, but a warning is given
 * when the desired section of `new' doesn't correspond with the section of
 * `old'. If a common definition is given again for a name, we take the
 * greatest value so that the common declared name always has enough space.
 * If a common is defined as a not-common, the old definition is ignored.
 */
static
redefine(new, old)
	register struct outname	*new, *old;
{
	if (!ISCOMMON(old)) {
		if (!ISCOMMON(new))
			error("%s: multiply defined", new->on_mptr);

		if ((new->on_type & S_TYP) != (old->on_type & S_TYP))
			warning("%s: sections differ", new->on_mptr);
	} else {
		/* `Old' is common. */
		if ((new->on_type & S_TYP) != (old->on_type & S_TYP))
			warning("%s: sections differ", new->on_mptr);

		if (ISCOMMON(new)) {
			if (new->on_valu > old->on_valu)
				old->on_valu = new->on_valu;
		} else {
			transfer(new, old);
		}
	}
}

/*
 * Transfer things we want to know from `src' to `dst'.
 */
static
transfer(src, dst)
	register struct outname	*src, *dst;
{
	debug("%s defined here\n", src->on_mptr, 0, 0, 0);
	dst->on_valu = src->on_valu;
	dst->on_type = src->on_type;
	dst->on_desc = src->on_desc;
}
