#ifndef lint
static char rcsid[] = "$Header$";
#endif

#include "../../h/out.h"
#include "const.h"
#include "debug.h"
#include "defs.h"
#include "orig.h"

#define UBYTE(x)	((x) & BYTEMASK)

/*
 * The bits in type indicate how many bytes the value occupies and what
 * significance should be attributed to each byte.
 */
static long
getvalu(addr, type)
	char	addr[];
	char	type;
{
	ushort	word0, word1;

	switch (type & RELSZ) {
	case RELO1:
		return UBYTE(addr[0]);
	case RELO2:
		if (type & RELBR)
			return (UBYTE(addr[0]) << WIDTH) + UBYTE(addr[1]);
		else
			return (UBYTE(addr[1]) << WIDTH) + UBYTE(addr[0]);
	case RELO4:
		if (type & RELBR) {
			word0 = (UBYTE(addr[0]) << WIDTH) + UBYTE(addr[1]);
			word1 = (UBYTE(addr[2]) << WIDTH) + UBYTE(addr[3]);
		} else {
			word0 = (UBYTE(addr[1]) << WIDTH) + UBYTE(addr[0]);
			word1 = (UBYTE(addr[3]) << WIDTH) + UBYTE(addr[2]);
		}
		if (type & RELWR)
			return ((long)word0 << (2 * WIDTH)) + word1;
		else
			return ((long)word1 << (2 * WIDTH)) + word0;
	default:
		fatal("bad relocation size");
	}
	/* NOTREACHED */
}

/*
 * The bits in type indicate how many bytes the value occupies and what
 * significance should be attributed to each byte.
 * We do not check for overflow.
 */
static
putvalu(valu, addr, type)
	long	valu;
	char	addr[];
	char	type;
{
	ushort	word0, word1;

	switch (type & RELSZ) {
	case RELO1:
		addr[0] = valu;
		break;
	case RELO2:
		if (type & RELBR) {
			addr[0] = valu >> WIDTH;
			addr[1] = valu;
		} else {
			addr[0] = valu;
			addr[1] = valu >> WIDTH;
		}
		break;
	case RELO4:
		if (type & RELWR) {
			word0 = valu >> (2 * WIDTH);
			word1 = valu;
		} else {
			word0 = valu;
			word1 = valu >> (2 * WIDTH);
		}
		if (type & RELBR) {
			addr[0] = word0 >> WIDTH;
			addr[1] = word0;
			addr[2] = word1 >> WIDTH;
			addr[3] = word1;
		} else {
			addr[0] = word0;
			addr[1] = word0 >> WIDTH;
			addr[2] = word1;
			addr[3] = word1 >> WIDTH;
		}
		break;
	default:
		fatal("bad relocation size");
	}
}

/*
 * Returns whether `valu' refers to the zero part of its section.
 * The address of its zero part (relative to the beginning of the section)
 * is in `zero_addr'. If `valu' is used in a pc-relative address computation,
 * we have to do that computation ourselves. A pc-relative address is the
 * difference between the address of the byte after the value and the "real"
 * address:
 * referencing address + its size + pc-relative address == "real" address.
 */
static bool
refers_zero(valu, relo, zero_addr)
	register long	valu;
	struct outrelo	*relo;
	long		zero_addr;
{
	if (relo->or_type & RELPC) {
		valu += relo->or_addr;
		/*
		 * Below is a dirty switch-statement. But an even dirtier
		 * statement would be: valu += (relo->or_type & RELSZ),
		 * because in that case you would have to know the values
		 * of the RELO[124] symbols.
		 */
		switch (relo->or_type & RELSZ) {
		case RELO4:	valu += 1;
				valu += 1;
		case RELO2:	valu += 1;
		case RELO1:	valu += 1;
		}
	}
	return valu >= zero_addr;
}

extern ushort		NLocals, NGlobals;
extern struct outsect	outsect[];
extern struct orig	relorig[];

/*
 * There are two cases: `local' is an undefined external or common name,
 * or `local' is a section name.
 * First case: if the name has been defined in another module,
 * its value is known and can be added. Or_nami will be the
 * index of the name of the section in which this name was
 * defined. Otherwise we must change or_nami to the index of
 * this name in the name table of the output file and leave
 * its value unchanged.
 * Second case: we must update the value by the change
 * in position of the section of local.
 */
static ushort
addrelo(relo, names, sects, valu_out)
	struct outrelo		*relo;
	struct outname		*names;
	struct outsect		*sects;
	long			*valu_out;	/* Out variable. */
{
	register struct outname	*local = &names[relo->or_nami];
	register ushort		index = NLocals;
	register long		valu = *valu_out;

	if ((local->on_type & S_SCT)) {
		register int	sectindex = (local->on_type & S_TYP) - S_MIN;

		if (refers_zero(valu, relo, sects[sectindex].os_flen)) {
			valu -= sects[sectindex].os_flen;
			valu += outsect[sectindex].os_flen;
			valu += relorig[sectindex].org_zero;
		} else {
			valu += relorig[sectindex].org_flen;
		}
		valu += outsect[sectindex].os_base;
		index += NGlobals + sectindex;
	} else {
		register struct outname	*name;
		extern int		hash();
		extern struct outname	*searchname();
		extern ushort		indexof();

		name = searchname(local->on_mptr, hash(local->on_mptr));
		if (name == (struct outname *)0)
			fatal("name %s not found in pass 2", local->on_mptr);
		if (ISCOMMON(name) || ISUNDEFINED(name)) {
			debug("can't relocate from %s\n",local->on_mptr,0,0,0);
			index += indexof(name);
		} else {
			valu += name->on_valu;
			index += NGlobals + (name->on_type & S_TYP) - S_MIN;
		}
	}
	*valu_out = valu;
	return index;
}

/*
 * This routine relocates a value in a section pointed to by `emit', of
 * which the header is pointed to by `head'. Relocation is relative to the
 * names in `names'; `relo' tells how to relocate.
 */
relocate(head, emit, names, relo, sects)
	struct outhead	*head;
	char		*emit;
	struct outname	names[];
	struct outrelo	*relo;
	struct outsect	*sects;
{
	long		valu;
	int		sectindex = relo->or_sect - S_MIN;
	extern struct outhead	outhead;

	/*
	 * Pick up previous value at location to be relocated.
	 */
	valu = getvalu(emit + relo->or_addr, relo->or_type);
	/*
	 * Or_nami is an index in the name table of the considered module.
	 * The name of which it is an index can be:
	 *	- an undefined external or a common name
	 *	- a section name
	 *	- the first name outside! the name table (argh)
	 */
	if (relo->or_nami < head->oh_nname) {
		/* First two cases. */
		relo->or_nami = addrelo(relo, names, sects, &valu);
	} else {
		/*
		 * Third case: it is absolute. The relocation of absolute
		 * names is always 0. We only need to change the index.
		 */
		relo->or_nami = NLocals + NGlobals + outhead.oh_nsect;
	}

	/*
	 * If relocation is pc-relative, we had to update the value by
	 * the change in distance between the referencING and referencED
	 * section. We already added the origin of the referencED section;
	 * now we subtract the origin of the referencING section.
	 * Note that the the value to be relocated cannot lie within the
	 * zero part.
	 */
	if (relo->or_type & RELPC)
		valu -=	relorig[sectindex].org_flen+outsect[sectindex].os_base;

	/*
	 * Now put the value back.
	 */
	putvalu(valu, emit + relo->or_addr, relo->or_type);

	/*
	 * We must change the offset within the section of the value to be
	 * relocated to its offset in the new section. `Or_addr' must again be
	 * in the normal part, of course.
	 */
	relo->or_addr += relorig[sectindex].org_flen;
}
