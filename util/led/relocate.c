/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "out.h"
#include "const.h"
#include "debug.h"
#include "defs.h"
#include "orig.h"
#include "sym.h"
#include "relocate.h"

#define UBYTE(x) ((x)&BYTEMASK)

static uint16_t read2(char* addr, int type)
{
	if (type & RELBR)
		return (UBYTE(addr[0]) << WIDTH) + UBYTE(addr[1]);
	else
		return (UBYTE(addr[1]) << WIDTH) + UBYTE(addr[0]);
}

static uint32_t read3(char* addr, int type)
{
	if (type & RELBR)
		return (UBYTE(addr[0]) << (WIDTH * 2)) + (UBYTE(addr[1]) << (WIDTH * 1))
		    + UBYTE(addr[1]);
	else
		return (UBYTE(addr[2]) << (WIDTH * 2)) + (UBYTE(addr[1]) << (WIDTH * 1))
		    + UBYTE(addr[0]);
}

static uint32_t read4(char* addr, int type)
{
	unsigned short word0, word1;

	if (type & RELBR)
	{
		word0 = (UBYTE(addr[0]) << WIDTH) + UBYTE(addr[1]);
		word1 = (UBYTE(addr[2]) << WIDTH) + UBYTE(addr[3]);
	}
	else
	{
		word0 = (UBYTE(addr[1]) << WIDTH) + UBYTE(addr[0]);
		word1 = (UBYTE(addr[3]) << WIDTH) + UBYTE(addr[2]);
	}
	if (type & RELWR)
		return ((long)word0 << (2 * WIDTH)) + word1;
	else
		return ((long)word1 << (2 * WIDTH)) + word0;
}

/* VideoCore 4 fixups are complex as we need to patch the instruction in
 * one of several different ways (depending on what the instruction is).
 */

static uint32_t get_vc4_valu(char* addr)
{
	uint16_t opcode = read2(addr, 0);

	if ((opcode & 0xff00) == 0xe700)
	{
		/* ld<w> rd, $+o:  [1110 0111 ww 0 d:5] [11111 o:27]
		 * st<w> rd, $+o:  [1110 0111 ww 1 d:5] [11111 o:27]
		 */

		int32_t value = read4(addr + 2, 0);
		value &= 0x07ffffff;
		value = value << 5 >> 5;
		return value;
	}

	if ((opcode & 0xf080) == 0x9000)
	{
		/* b<cc> $+o*2:  [1001 cccc 0ooo oooo] [oooo oooo oooo oooo]
		 * Yes, big-endian (the first 16 bits is the MSB).
		 */

		uint32_t value = read4(addr, RELWR);
		value &= 0x007fffff;
		value = value << 9 >> 9;
		value *= 2;
		return value;
	}

	if ((opcode & 0xf080) == 0x9080)
	{
		/* bl $+o*2:  [1001 oooo 1ooo oooo] [oooo oooo oooo oooo]
		 * Yes, big-endian (the first 16 bits is the MSB).
		 * (Note that o is split.)
		 */

		int32_t value = read4(addr, RELWR);
		int32_t lov = value & 0x007fffff;
		int32_t hiv = value & 0x0f000000;
		value = lov | (hiv >> 1);
		value = value << 5 >> 5;
		value *= 2;
		return value;
	}

	if ((opcode & 0xffe0) == 0xe500)
	{
		/* lea: [1110 0101 000 d:5] [o:32] */

		return read4(addr + 2, 0);
	}

	assert(0 && "unrecognised VC4 instruction");
}

static bool is_powerpc_memory_op(uint32_t opcode)
{
	/* Tests for any PowerPC memory indirection instruction (or
	 * addi) where the payload is a *signed* 16-bit value. */
	switch ((opcode & 0xfc000000) >> 26)
	{
		case 14: /* addi */
		case 34: /* lbz */
		case 48: /* lfs */
		case 50: /* lfd */
		case 42: /* lha */
		case 40: /* lhz */
		case 32: /* lwz */
		case 38: /* stb */
		case 52: /* stfs */
		case 54: /* stfd */
		case 44: /* sth */
		case 36: /* stw */
			return true;
	}

	return false;
}

/* PowerPC fixups are complex as we need to patch up to the next two
 * instructions in one of several different ways, depending on what the
 * instructions area.
 */

static uint32_t get_powerpc_valu(char* addr, uint16_t type)
{
	uint32_t opcode1 = read4(addr + 0, type);
	uint32_t opcode2 = read4(addr + 4, type);

	if ((opcode1 & 0xfc000000) == 0x48000000)
	{
		/* branch instruction */
		return opcode1 & 0x03fffffd;
	}
	else if (
	    ((opcode1 & 0xfc1f0000) == 0x3c000000)
	    && ((opcode2 & 0xfc000000) == 0x60000000))
	{
		/* addis / ori instruction pair */
		return ((opcode1 & 0xffff) << 16) | (opcode2 & 0xffff);
	}
	else if (
	    ((opcode1 & 0xfc1f0000) == 0x3c000000) && is_powerpc_memory_op(opcode2))
	{
		/* addis / memoryop instruction pair */
		uint16_t hi = opcode1 & 0xffff;
		uint16_t lo = opcode2 & 0xffff;

		/* Undo the sign adjustment (see mach/powerpc/as/mach5.c). */

		if (lo > 0x7fff)
			hi--;

		return ((hi << 16) | lo);
	}

	fatal(
	    "Don't know how to read from PowerPC fixup on instructions "
	    "0x%08lx+0x%08lx",
	    (unsigned long)opcode1, (unsigned long)opcode2);
}

/* RELOPPC_LIS stores a signed 26-bit offset in the low bits. */
static uint32_t get_lis_valu(char* addr, uint16_t type)
{
	uint32_t valu = read4(addr, type) & 0x03ffffff;
	if (valu & 0x02000000)
		valu |= 0xfc000000; /* sign extension */
	return valu;
}

/* RELOMIPS is used for j and b instructions only. */
static uint32_t get_mips_valu(char* addr)
{
	uint32_t value = read4(addr, 0);
	switch (value >> 26)
	{
		case 2: /* j */
		case 3: /* jal */
		case 29: /* jalx */
			/* Unsigned 26-bit payload. */
			value = value & ((1 << 26) - 1);
			break;

		default: /* assume everything else is a b, there are lots */
			/* Signed 16-bit payload. */
			value = ((int32_t)value << 16) >> 16;
			break;
	}

	/* The value has two implicit zero bits on the bottom. */
	value <<= 2;
	return value;
}

/*
 * The bits in type indicate how many bytes the value occupies and what
 * significance should be attributed to each byte.
 */
static uint32_t getvalu(char* addr, uint16_t type)
{
	switch (type & RELSZ)
	{
		case RELO1:
			return UBYTE(addr[0]);
		case RELO2:
		case RELO2HI:
		case RELO2HISAD:
			return read2(addr, type);
		case RELO3:
			return read3(addr, type);
		case RELO4:
			return read4(addr, type);
		case RELOPPC:
			return get_powerpc_valu(addr, type);
		case RELOPPC_LIS:
			return get_lis_valu(addr, type);
		case RELOVC4:
			return get_vc4_valu(addr);
		case RELOMIPS:
			return get_mips_valu(addr);
		default:
			fatal("can't read relocation type %x", type & RELSZ);
	}
	/* NOTREACHED */
}

static void write2(uint16_t valu, char* addr, int type)
{
	if (type & RELBR)
	{
		addr[0] = valu >> WIDTH;
		addr[1] = valu;
	}
	else
	{
		addr[0] = valu;
		addr[1] = valu >> WIDTH;
	}
}

static void write3(uint16_t valu, char* addr, int type)
{
	if (type & RELBR)
	{
		addr[0] = valu >> (WIDTH * 2);
		addr[1] = valu >> (WIDTH * 1);
		addr[2] = valu;
	}
	else
	{
		addr[0] = valu;
		addr[1] = valu >> (WIDTH * 1);
		addr[2] = valu >> (WIDTH * 2);
	}
}

static void write4(uint32_t valu, char* addr, int type)
{
	unsigned short word0, word1;

	if (type & RELWR)
	{
		word0 = valu >> (2 * WIDTH);
		word1 = valu;
	}
	else
	{
		word0 = valu;
		word1 = valu >> (2 * WIDTH);
	}
	if (type & RELBR)
	{
		addr[0] = word0 >> WIDTH;
		addr[1] = word0;
		addr[2] = word1 >> WIDTH;
		addr[3] = word1;
	}
	else
	{
		addr[0] = word0;
		addr[1] = word0 >> WIDTH;
		addr[2] = word1;
		addr[3] = word1 >> WIDTH;
	}
}

/* VideoCore 4 fixups are complex as we need to patch the instruction in
 * one of several different ways (depending on what the instruction is).
 */

static void put_vc4_valu(char* addr, uint32_t value)
{
	uint16_t opcode = read2(addr, 0);

	if ((opcode & 0xff00) == 0xe700)
	{
		/* ld<w> rd, o, (pc):  [1110 0111 ww 0 d:5] [11111 o:27]
		 * st<w> rd, o, (pc):  [1110 0111 ww 1 d:5] [11111 o:27]
		 */

		uint32_t v = read4(addr + 2, 0);
		v &= 0xf8000000;
		v |= value & 0x07ffffff;
		write4(v, addr + 2, 0);
	}
	else if ((opcode & 0xf080) == 0x9000)
	{
		/* b<cc> dest:  [1001 cccc 0ooo oooo] [oooo oooo oooo oooo]
		 * Yes, big-endian (the first 16 bits is the MSB).
		 */

		uint32_t v = read4(addr, RELWR);
		v &= 0xff800000;
		v |= (value / 2) & 0x007fffff;
		write4(v, addr, RELWR);
	}
	else if ((opcode & 0xf080) == 0x9080)
	{
		/* bl dest:  [1001 oooo 1ooo oooo] [oooo oooo oooo oooo]
		 * Yes, big-endian (the first 16 bits is the MSB).
		 * (Note that o is split.)
		 */

		uint32_t v = read4(addr, RELWR);
		uint32_t lovalue = (value / 2) & 0x007fffff;
		uint32_t hivalue = (value / 2) & 0x07800000;
		v &= 0xf0800000;
		v |= lovalue | (hivalue << 1);
		write4(v, addr, RELWR);
	}
	else if ((opcode & 0xffe0) == 0xe500)
	{
		/* lea: [1110 0101 000 d:5] [o:32] */

		write4(value, addr + 2, 0);
	}
	else
		assert(0 && "unrecognised VC4 instruction");
}

/* PowerPC fixups are complex as we need to patch up to the next two
 * instructions in one of several different ways, depending on what the
 * instructions area.
 */

static void put_powerpc_valu(char* addr, uint32_t value, uint16_t type)
{
	uint32_t opcode1 = read4(addr + 0, type);
	uint32_t opcode2 = read4(addr + 4, type);

	if ((opcode1 & 0xfc000000) == 0x48000000)
	{
		/* branch instruction */
		uint32_t i = opcode1 & ~0x03fffffd;
		i |= value & 0x03fffffd;
		write4(i, addr, type);
	}
	else if (
	    ((opcode1 & 0xfc1f0000) == 0x3c000000)
	    && ((opcode2 & 0xfc000000) == 0x60000000))
	{
		/* addis / ori instruction pair */
		uint16_t hi = value >> 16;
		uint16_t lo = value & 0xffff;

		write4((opcode1 & 0xffff0000) | hi, addr + 0, type);
		write4((opcode2 & 0xffff0000) | lo, addr + 4, type);
	}
	else if (
	    ((opcode1 & 0xfc1f0000) == 0x3c000000) && is_powerpc_memory_op(opcode2))
	{
		/* addis / memoryop instruction pair */
		uint16_t hi = value >> 16;
		uint16_t lo = value & 0xffff;

		/* Apply the sign adjustment (see mach/powerpc/as/mach5.c). */

		if (lo > 0x7fff)
			hi++;

		write4((opcode1 & 0xffff0000) | hi, addr + 0, type);
		write4((opcode2 & 0xffff0000) | lo, addr + 4, type);
	}

	else
		fatal(
		    "Don't know how to write a PowerPC fixup to instructions "
		    "0x%08lx+0x%08lx",
		    (unsigned long)opcode1, (unsigned long)opcode2);
}

/* Writes a PowerPC lis instruction. */
static void put_lis_valu(char* addr, uint32_t value, uint16_t type)
{
	uint32_t opcode, reg;
	uint16_t hi, lo;
	bool ha16;

	/* ha16 flag in high bit, register in next 5 bits */
	opcode = read4(addr, type);
	ha16 = opcode >> 31;
	reg = (opcode >> 26) & 0x1f;

	/*
	 * Apply the sign adjustment if the ha16 flag is set and the
	 * low half is a negative signed 16-bit integer.
	 */
	hi = value >> 16;
	lo = value & 0xffff;
	if (ha16 && lo > 0x7fff)
		hi++;

	/* Assemble lis reg, hi == addis reg, r0, hi. */
	opcode = (15 << 26) | (reg << 21) | (0 << 16) | hi;
	write4(opcode, addr, type);
}

/* RELOMIPS is used for j and b instructions only. */
static void put_mips_valu(char* addr, uint32_t value)
{
	uint32_t opcode = read4(addr, 0);

	/* The two bottom zero bits are implicit. */
	if (value & 3)
		fatal("invalid MIPS relocation value 0x%lx", (unsigned long)value);
	value >>= 2;

	switch (opcode >> 26)
	{
		case 2: /* j */
		case 3: /* jal */
		case 29: /* jalx */
			/* Unsigned 26-bit payload. */
			value = value & ((1 << 26) - 1);
			opcode = opcode & ~((1 << 26) - 1);
			break;

		default: /* assume everything else is a b, there are lots */
			/* Signed 16-bit payload. */
			value = value & ((1 << 16) - 1);
			opcode = opcode & ~((1 << 16) - 1);
			break;
	}

	write4(opcode | value, addr, 0);
}

/*
 * The bits in type indicate how many bytes the value occupies and what
 * significance should be attributed to each byte.
 * We do not check for overflow.
 */
static void putvalu(uint32_t valu, char* addr, uint16_t type)
{

	switch (type & RELSZ)
	{
		case RELO1:
			addr[0] = valu;
			break;
		case RELO2:
			write2(valu, addr, type);
			break;
		case RELO3:
			write3(valu, addr, type);
			break;
		case RELO2HI:
			write2(valu >> 16, addr, type);
			break;
		case RELO2HISAD:
			write2((valu >> 16) + !!(valu & 0x8000), addr, type);
			break;
		case RELO4:
			write4(valu, addr, type);
			break;
		case RELOPPC:
			put_powerpc_valu(addr, valu, type);
			break;
		case RELOPPC_LIS:
			put_lis_valu(addr, valu, type);
			break;
		case RELOVC4:
			put_vc4_valu(addr, valu);
			break;
		case RELOMIPS:
			put_mips_valu(addr, valu);
			break;
		default:
			fatal("can't write relocation type %x", type & RELSZ);
	}
}

extern struct outsect outsect[];
extern struct orig relorig[];

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
static unsigned addrelo(relo, names, valu_out)
struct outrelo* relo;
struct outname* names;
long* valu_out; /* Out variable. */
{
	register struct outname* local = &names[relo->or_nami];
	register unsigned short index = NLocals;
	register long valu = *valu_out;

	if ((local->on_type & S_SCT))
	{
		register int sectindex = (local->on_type & S_TYP) - S_MIN;

		valu += relorig[sectindex].org_size;
		valu += outsect[sectindex].os_base;
		index += NGlobals + sectindex;
	}
	else
	{
		register struct outname* name;
		extern int hash();
		extern struct outname* searchname();
		extern unsigned indexof();
		extern struct outhead outhead;

		name = searchname(local->on_mptr, hash(local->on_mptr));
		if (name == (struct outname*)0)
			fatal("name %s not found in pass 2", local->on_mptr);
		if (ISCOMMON(name) || ISUNDEFINED(name))
		{
			debug("can't relocate from %s\n", local->on_mptr, 0, 0, 0);
			index += indexof(name);
		}
		else
		{
			valu += name->on_valu;
			if ((name->on_type & S_TYP) == S_ABS)
			{
				index += NGlobals + outhead.oh_nsect;
			}
			else
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
void relocate(
    struct outhead* head,
    char* emit,
    struct outname names[],
    struct outrelo* relo,
    long off)
{
	long valu;
	int sectindex = relo->or_sect - S_MIN;
	extern struct outhead outhead;
	uint32_t realaddress = outsect[sectindex].os_base + relo->or_addr
	    + relorig[sectindex].org_size;

	/*
	 * Pick up previous value at location to be relocated.
	 */
	valu = getvalu(emit + (relo->or_addr - off), relo->or_type);
	debug(
	    "read relocation from 0x%08lx type 0x%x value 0x%08lx symbol %u\n",
	    (unsigned long)realaddress, relo->or_type, valu, relo->or_nami);

	/*
	 * Or_nami is an index in the name table of the considered module.
	 * The name of which it is an index can be:
	 *	- an undefined external or a common name
	 *	- a section name
	 *	- the first name outside! the name table (argh)
	 */
	if (relo->or_nami < head->oh_nname)
	{
		/* First two cases. */
		relo->or_nami = addrelo(relo, names, &valu);
	}
	else
	{
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
		valu -= relorig[sectindex].org_size + outsect[sectindex].os_base;

	/*
	 * Now put the value back.
	 */
	debug(
	    "written fixed up relocation to 0x%08lx type 0x%x value 0x%08lx\n",
	    (unsigned long)realaddress, relo->or_type, valu, 0);
	putvalu(valu, emit + (relo->or_addr - off), relo->or_type);

	/*
	 * We must change the offset within the section of the value to be
	 * relocated to its offset in the new section. `Or_addr' must again be
	 * in the normal part, of course.
	 */
	relo->or_addr += relorig[sectindex].org_size;
}
