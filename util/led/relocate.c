/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <stdlib.h>
#include <stdio.h>
#include "out.h"
#include "const.h"
#include "debug.h"
#include "defs.h"
#include "orig.h"

#define UBYTE(x)	((x) & BYTEMASK)

static long read2(char* addr, int type)
{
	unsigned short word0, word1;

	if (type & RELBR)
		return (UBYTE(addr[0]) << WIDTH) + UBYTE(addr[1]);
	else
		return (UBYTE(addr[1]) << WIDTH) + UBYTE(addr[0]);
}

static long read4(char* addr, int type)
{
	unsigned short word0, word1;

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
}

/*
 * The bits in type indicate how many bytes the value occupies and what
 * significance should be attributed to each byte.
 */
static long
getvalu(addr, type)
	char	addr[];
	char	type;
{
	switch (type & RELSZ) {
	case RELO1:
		return UBYTE(addr[0]);
	case RELO2:
		return read2(addr, type);
	case RELO4:
		return read4(addr, type);
	case RELOPPC:
		return read4(addr, type) & 0x03FFFFFD;
	case RELOH2:
		return read2(addr, type) << 16;
	default:
		fatal("bad relocation size");
	}
	/* NOTREACHED */
}

static void write2(long valu, char* addr, int type)
{
	unsigned short	word0, word1;

	if (type & RELBR) {
		addr[0] = valu >> WIDTH;
		addr[1] = valu;
	} else {
		addr[0] = valu;
		addr[1] = valu >> WIDTH;
	}
}

static void write4(long valu, char* addr, int type)
{
	unsigned short	word0, word1;

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

	switch (type & RELSZ) {
	case RELO1:
		addr[0] = valu;
		break;
	case RELO2:
		write2(valu, addr, type);
		break;
	case RELO4:
		write4(valu, addr, type);
		break;
	case RELOPPC:
	{
		long i = read4(addr, type) & ~0x03FFFFFD;
		i |= valu & 0x03FFFFFD;
		write4(i, addr, type);
		break;
	}
	case RELOH2:
		write2(valu>>16, addr, type);
		break;
	default:
		fatal("bad relocation size");
	}
}

extern unsigned short	NLocals, NGlobals;
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
static unsigned
addrelo(relo, names, valu_out)
	struct outrelo		*relo;
	struct outname		*names;
	long			*valu_out;	/* Out variable. */
{
	register struct outname	*local = &names[relo->or_nami];
	register unsigned short		index = NLocals;
	register long		valu = *valu_out;

	if ((local->on_type & S_SCT)) {
		register int	sectindex = (local->on_type & S_TYP) - S_MIN;

		valu += relorig[sectindex].org_size;
		valu += outsect[sectindex].os_base;
		index += NGlobals + sectindex;
	} else {
		register struct outname	*name;
		extern int		hash();
		extern struct outname	*searchname();
		extern unsigned 	indexof();
		extern struct outhead	outhead; 

		name = searchname(local->on_mptr, hash(local->on_mptr));
		if (name == (struct outname *)0)
			fatal("name %s not found in pass 2", local->on_mptr);
		if (ISCOMMON(name) || ISUNDEFINED(name)) {
			debug("can't relocate from %s\n",local->on_mptr,0,0,0);
			index += indexof(name);
		} else {
			valu += name->on_valu;
			if ((name->on_type & S_TYP) == S_ABS) {
				index += NGlobals + outhead.oh_nsect;
			}
			else	index += NGlobals +
					(name->on_type & S_TYP) - S_MIN;
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
relocate(head, emit, names, relo, off)
	struct outhead	*head;
	char		*emit;
	struct outname	names[];
	struct outrelo	*relo;
	long		off;
{
	long		valu;
	int		sectindex = relo->or_sect - S_MIN;
	extern struct outhead	outhead;

	/*
	 * Pick up previous value at location to be relocated.
	 */
	valu = getvalu(emit + (relo->or_addr - off), relo->or_type);

	/*
	 * Or_nami is an index in the name table of the considered module.
	 * The name of which it is an index can be:
	 *	- an undefined external or a common name
	 *	- a section name
	 *	- the first name outside! the name table (argh)
	 */
	if (relo->or_nami < head->oh_nname) {
		/* First two cases. */
		relo->or_nami = addrelo(relo, names, &valu);
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
	 */
	if (relo->or_type & RELPC)
		valu -=	relorig[sectindex].org_size+outsect[sectindex].os_base;

	/*
	 * Now put the value back.
	 */
	putvalu(valu, emit + (relo->or_addr - off), relo->or_type);

	/*
	 * We must change the offset within the section of the value to be
	 * relocated to its offset in the new section. `Or_addr' must again be
	 * in the normal part, of course.
	 */
	relo->or_addr += relorig[sectindex].org_size;
}
