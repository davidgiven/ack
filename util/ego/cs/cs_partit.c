/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* Functions to partition the huge set of EM-instructions. */

#include <em_mnem.h>
#include <em_pseu.h>
#include <em_reg.h>
#include <em_spec.h>
#include "../share/types.h"
#include "../share/aux.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "cs.h"
#include "cs_stack.h"

#define XXX	(-1)
#define ARGW	0
#define WS	1
#define PS	2
#define FEF	3
#define FIF	4
#define CVT	5

#define ANY	0
#define PTR	1
#define FLT	2

STATIC struct {
    byte i_group;   /* Group of instruction. */
    byte i_op1;		/* Indication of size of operand of unary operator. */
			/* Idem for 1st operand of binary operator. */
    byte i_op2;		/* Idem for 2nd operand of binary operator. */
    byte i_av;		/* Idem for result of operators. */
    byte i_regtype;	/* ANY, PTR, FLT. */
} info[] = {
		XXX,		XXX,	XXX,	XXX,	XXX,
/* aar */	TERNAIR_OP,	XXX,	XXX,	PS,	PTR,
/* adf */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	FLT,
/* adi */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* adp */	EXPENSIVE_LOAD,	XXX,	XXX,	XXX,	PTR,
/* ads */	BINAIR_OP,	PS,	ARGW,	PS,	PTR,
/* adu */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* and */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* asp */	FIDDLE_STACK,	XXX,	XXX,	XXX,	XXX,
/* ass */	FIDDLE_STACK,	XXX,	XXX,	XXX,	XXX,
/* beq */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* bge */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* bgt */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* ble */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* blm */	HOPELESS,	XXX,	XXX,	XXX,	XXX,
/* bls */	HOPELESS,	XXX,	XXX,	XXX,	XXX,
/* blt */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* bne */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* bra */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* cai */	SIDE_EFFECTS,	XXX,	XXX,	XXX,	XXX,
/* cal */	SIDE_EFFECTS,	XXX,	XXX,	XXX,	XXX,
/* cff */	TERNAIR_OP,	XXX,	XXX,	CVT,	FLT,
/* cfi */	TERNAIR_OP,	XXX,	XXX,	CVT,	ANY,
/* cfu */	TERNAIR_OP,	XXX,	XXX,	CVT,	ANY,
/* cif */	TERNAIR_OP,	XXX,	XXX,	CVT,	FLT,
/* cii */	TERNAIR_OP,	XXX,	XXX,	CVT,	ANY,
/* ciu */	TERNAIR_OP,	XXX,	XXX,	CVT,	ANY,
/* cmf */	BINAIR_OP,	ARGW,	ARGW,	WS,	ANY,
/* cmi */	BINAIR_OP,	ARGW,	ARGW,	WS,	ANY,
/* cmp */	BINAIR_OP,	PS,	PS,	WS,	ANY,
/* cms */	BINAIR_OP,	ARGW,	ARGW,	WS,	ANY,
/* cmu */	BINAIR_OP,	ARGW,	ARGW,	WS,	ANY,
/* com */	UNAIR_OP,	ARGW,	XXX,	ARGW,	ANY,
/* csa */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* csb */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* cuf */	TERNAIR_OP,	XXX,	XXX,	CVT,	FLT,
/* cui */	TERNAIR_OP,	XXX,	XXX,	CVT,	ANY,
/* cuu */	TERNAIR_OP,	XXX,	XXX,	CVT,	ANY,
/* dch */	UNAIR_OP,	PS,	XXX,	PS,	PTR,
/* dec */	UNAIR_OP,	WS,	XXX,	WS,	ANY,
/* dee */	KILL_ENTITY,	XXX,	XXX,	XXX,	XXX,
/* del */	KILL_ENTITY,	XXX,	XXX,	XXX,	XXX,
/* dup */	FIDDLE_STACK,	XXX,	XXX,	XXX,	XXX,
/* dus */	FIDDLE_STACK,	XXX,	XXX,	XXX,	XXX,
/* dvf */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	FLT,
/* dvi */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* dvu */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* exg */	FIDDLE_STACK,	XXX,	XXX,	XXX,	XXX,
/* fef */	UNAIR_OP,	ARGW,	XXX,	FEF,	XXX,
/* fif */	BINAIR_OP,	ARGW,	ARGW,	FIF,	XXX,
/* fil */	IGNORE,		XXX,	XXX,	XXX,	XXX,
/* gto */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* inc */	UNAIR_OP,	WS,	XXX,	WS,	ANY,
/* ine */	KILL_ENTITY,	XXX,	XXX,	XXX,	XXX,
/* inl */	KILL_ENTITY,	XXX,	XXX,	XXX,	XXX,
/* inn */	BINAIR_OP,	ARGW,	WS,	WS,	ANY,
/* ior */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* lae */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* lal */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* lar */	LOAD_ARRAY,	XXX,	XXX,	XXX,	ANY,
/* ldc */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* lde */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* ldf */	EXPENSIVE_LOAD,	XXX,	XXX,	XXX,	ANY,
/* ldl */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* lfr */	FIDDLE_STACK,	XXX,	XXX,	XXX,	XXX,
/* lil */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* lim */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* lin */	IGNORE,		XXX,	XXX,	XXX,	XXX,
/* lni */	IGNORE,		XXX,	XXX,	XXX,	XXX,
/* loc */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* loe */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* lof */	EXPENSIVE_LOAD,	XXX,	XXX,	XXX,	ANY,
/* loi */	EXPENSIVE_LOAD,	XXX,	XXX,	XXX,	ANY,
/* lol */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* lor */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* los */	FIDDLE_STACK,	XXX,	XXX,	XXX,	XXX,
/* lpb */	UNAIR_OP,	PS,	XXX,	PS,	PTR,
/* lpi */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* lxa */	EXPENSIVE_LOAD,	XXX,	XXX,	XXX,	PTR,
/* lxl */	EXPENSIVE_LOAD,	XXX,	XXX,	XXX,	PTR,
/* mlf */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	FLT,
/* mli */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* mlu */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* mon */	HOPELESS,	XXX,	XXX,	XXX,	XXX,
/* ngf */	UNAIR_OP,	ARGW,	XXX,	ARGW,	FLT,
/* ngi */	UNAIR_OP,	ARGW,	XXX,	ARGW,	ANY,
/* nop */	HOPELESS,	XXX,	XXX,	XXX,	XXX,
/* rck */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* ret */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* rmi */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* rmu */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* rol */	BINAIR_OP,	ARGW,	WS,	ARGW,	ANY,
/* ror */	BINAIR_OP,	ARGW,	WS,	ARGW,	ANY,
/* rtt */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* sar */	STORE_ARRAY,	XXX,	XXX,	XXX,	XXX,
/* sbf */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	FLT,
/* sbi */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* sbs */	BINAIR_OP,	PS,	PS,	ARGW,	ANY,
/* sbu */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* sde */	STORE_DIRECT,	XXX,	XXX,	XXX,	XXX,
/* sdf */	STORE_INDIR,	XXX,	XXX,	XXX,	XXX,
/* sdl */	STORE_DIRECT,	XXX,	XXX,	XXX,	XXX,
/* set */	UNAIR_OP,	WS,	XXX,	ARGW,	ANY,
/* sig */	FIDDLE_STACK,	XXX,	XXX,	XXX,	XXX,
/* sil */	STORE_INDIR,	XXX,	XXX,	XXX,	XXX,
/* sim */	STORE_DIRECT,	XXX,	XXX,	XXX,	XXX,
/* sli */	BINAIR_OP,	ARGW,	WS,	ARGW,	ANY,
/* slu */	BINAIR_OP,	ARGW,	WS,	ARGW,	ANY,
/* sri */	BINAIR_OP,	ARGW,	WS,	ARGW,	ANY,
/* sru */	BINAIR_OP,	ARGW,	WS,	ARGW,	ANY,
/* ste */	STORE_DIRECT,	XXX,	XXX,	XXX,	XXX,
/* stf */	STORE_INDIR,	XXX,	XXX,	XXX,	XXX,
/* sti */	STORE_INDIR,	XXX,	XXX,	XXX,	XXX,
/* stl */	STORE_DIRECT,	XXX,	XXX,	XXX,	XXX,
/* str */	HOPELESS,	XXX,	XXX,	XXX,	XXX,
/* sts */	HOPELESS,	XXX,	XXX,	XXX,	XXX,
/* teq */	UNAIR_OP,	WS,	XXX,	WS,	ANY,
/* tge */	UNAIR_OP,	WS,	XXX,	WS,	ANY,
/* tgt */	UNAIR_OP,	WS,	XXX,	WS,	ANY,
/* tle */	UNAIR_OP,	WS,	XXX,	WS,	ANY,
/* tlt */	UNAIR_OP,	WS,	XXX,	WS,	ANY,
/* tne */	UNAIR_OP,	WS,	XXX,	WS,	ANY,
/* trp */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* xor */	BINAIR_OP,	ARGW,	ARGW,	ARGW,	ANY,
/* zeq */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* zer */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* zge */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* zgt */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* zle */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* zlt */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* zne */	BBLOCK_END,	XXX,	XXX,	XXX,	XXX,
/* zre */	KILL_ENTITY,	XXX,	XXX,	XXX,	XXX,
/* zrf */	SIMPLE_LOAD,	XXX,	XXX,	XXX,	XXX,
/* zrl */	KILL_ENTITY,	XXX,	XXX,	XXX,	XXX
};

#define GROUP(n)	(info[n].i_group)
#define OP1SIZE(l)	(info[INSTR(l)].i_op1)
#define OP2SIZE(l)	(info[INSTR(l)].i_op2)
#define AVSIZE(l)	(info[INSTR(l)].i_av)
#define REGTYPE(n)	(info[n].i_regtype)

int instrgroup(lnp)
	line_p lnp;
{
	if (INSTR(lnp) == op_lor && SHORT(lnp) == 1) {
		/* We can't do anything with the stackpointer. */
		return FIDDLE_STACK;
	}
	if (INSTR(lnp) < sp_fmnem || INSTR(lnp) > sp_lmnem) {
		VI((short) INSTR(lnp));
		return IGNORE;
	}
	return GROUP(INSTR(lnp));
}

bool stack_group(instr)
	int instr;
{
	/* Is this an instruction that only does something to the top of
	 * the stack?
	 */
	switch (GROUP(instr)) {
		case SIMPLE_LOAD:
		case EXPENSIVE_LOAD:
		case LOAD_ARRAY:
		case UNAIR_OP:
		case BINAIR_OP:
		case TERNAIR_OP:
			return TRUE;
		default:
			return FALSE;
	}
}

STATIC offset argw(lnp)
	line_p lnp;
{
	/* Some EM-instructions have their argument either on the same line,
	 * or on top of the stack. We give up when the argument is on top of
	 * the stack.
	 */
	struct token dummy;

	if (TYPE(lnp) != OPNO) {
		return off_set(lnp);
	} else {
		Pop(&dummy, (offset) ws);
		return UNKNOWN_SIZE;
	}
}

offset op11size(lnp)
	line_p lnp;
{
	/* Returns the size of the first argument of
	 * the unary operator in lnp.
	 */

	switch (OP1SIZE(lnp)) {
		case ARGW:
			return argw(lnp);
		case WS:
			return ws;
		case PS:
			return ps;
		default:
			assert(FALSE);
	}
	/* NOTREACHED */
}

offset op12size(lnp)
	line_p lnp;
{
	/* Same for first of binary. */

	switch (OP1SIZE(lnp)) {
		case ARGW:
			return argw(lnp);
		case PS:
			return ps;
		default:
			assert(FALSE);
	}
	/* NOTREACHED */
}

offset op22size(lnp)
	line_p lnp;
{
	switch (OP2SIZE(lnp)) {
		case ARGW:
			return argw(lnp);
		case WS:
			return ws;
		case PS:
			return ps;
		default:
			assert(FALSE);
	}
	/* NOTREACHED */
}

/* Ternary operators are op_aar and conversions between types and/or sizes. */

offset op13size(lnp)
	line_p lnp;
{
	/* When the instruction is a conversion, the size of the first
	 * operand is the value of the second operand.
	 * We only handle the most likely case, namely that the second operand
	 * was pushed by a loc-instruction.
	 */
	if (INSTR(lnp) == op_aar) return ps;

	if (lnp->l_prev != (line_p) 0 &&
	    lnp->l_prev->l_prev != (line_p) 0 &&
	    INSTR(lnp->l_prev->l_prev) == op_loc
	   )
		return off_set(lnp->l_prev->l_prev);
	else
		return UNKNOWN_SIZE;
}

offset op23size(lnp)
	line_p lnp;
{
	if (INSTR(lnp) == op_aar)
		return argw(lnp);
	else
		return ws;
}

offset op33size(lnp)
	line_p lnp;
{
	if (INSTR(lnp) == op_aar)
		return ps;
	else
		return ws;
}

offset avsize(lnp)
	line_p lnp;
{
	/* Returns the size of the result of the instruction in lnp.
	 * If the instruction is a conversion this size is given on the stack.
	 * We only handle the case that this value was pushed by a loc.
	 */
	offset size;

	switch (AVSIZE(lnp)) {
		case ARGW:
			return argw(lnp);
		case WS:
			return ws;
		case PS:
			return ps;
		case FEF:
			if ((size = argw(lnp)) != UNKNOWN_SIZE)
				return size + ws;
			else
				return UNKNOWN_SIZE;
		case FIF:
			if ((size = argw(lnp)) != UNKNOWN_SIZE)
				return size + size;
			else
				return UNKNOWN_SIZE;
		case CVT:
			if (lnp->l_prev != (line_p) 0 &&
			    INSTR(lnp->l_prev) == op_loc
			   )
				return off_set(lnp->l_prev);
			else
				return UNKNOWN_SIZE;
		default:
			assert(FALSE);
			break;
	}
	/* NOTREACHED */
}

int regtype(instr)
	byte instr;
{
	switch (REGTYPE(instr & BMASK)) {
		case ANY:
			return reg_any;
		case PTR:
			return reg_pointer;
		case FLT:
			return reg_float;
		default:
			assert(FALSE);
	}
	/* NOTREACHED */
}
