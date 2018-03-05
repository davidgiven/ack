/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* V A L U E N U M B E R I N G   M E T H O D */

#include <em_mnem.h>
#include "../share/types.h"
#include "../share/global.h"
#include "../share/debug.h"
#include "../share/aux.h"
#include "cs.h"
#include "cs_alloc.h"
#include "cs_aux.h"
#include "cs_entity.h"
#include "cs_avail.h"
#include "cs_stack.h"
#include "cs_kill.h"
#include "cs_partit.h"
#include "cs_getent.h"
#include "cs_profit.h"

STATIC void push_entity(entity_p enp, line_p lfirst)
{
	/* Build token and Push it. */

	struct token tk;

	tk.tk_vn = enp->en_vn;
	tk.tk_size = enp->en_size;
	tk.tk_lfirst = lfirst;
	Push(&tk);
}

STATIC void put_expensive_load(bblock_p bp, line_p lnp, line_p lfirst,
			       entity_p enp)
{
	struct avail av;
	occur_p	ocp;

	av.av_instr = INSTR(lnp);
	av.av_size = enp->en_size;
	av.av_operand = enp->en_vn;

	ocp = newoccur(lfirst, lnp, bp);

	av_enter(&av, ocp, EXPENSIVE_LOAD);
}

STATIC void put_aar(bblock_p bp, line_p lnp, line_p lfirst, entity_p enp)
{
	/* Enter the implicit AAR in a LAR or SAR, where enp points to
	 * the ENARRELEM, and AAR computes its address.
	 */
	struct avail av;
	occur_p	ocp;

	assert(INSTR(lnp) == op_lar || INSTR(lnp) == op_sar);
	assert(enp->en_kind == ENARRELEM);
	av.av_instr = op_aar;
	av.av_size = ps;
	av.av_ofirst = enp->en_arbase;
	av.av_osecond = enp->en_index;
	av.av_othird = enp->en_adesc;

	/* Before we enter an available AAR, we must check whether we
	 * may convert this LAR/SAR to AAR LOI/STI.  This is so we
	 * don't LOI/STI a large or unknown size.
	 */
	if (may_become_aar(&av)) {
		ocp = newoccur(lfirst, lnp, bp);
		av_enter(&av, ocp, TERNAIR_OP);
	}
}

STATIC void push_avail(avail_p avp, line_p lfirst)
{
	struct token tk;

	tk.tk_vn = avp->av_result;
	tk.tk_size = avp->av_size;
	tk.tk_lfirst = lfirst;
	Push(&tk);
}

STATIC void push_unair_op(bblock_p bp, line_p lnp, token_p tkp1)
{
	struct avail av;
	occur_p	ocp;

	av.av_instr = INSTR(lnp);
	av.av_size = avsize(lnp);
	av.av_operand = tkp1->tk_vn;

	ocp = newoccur(tkp1->tk_lfirst, lnp, bp);

	push_avail(av_enter(&av, ocp, UNAIR_OP), tkp1->tk_lfirst);
}

STATIC void push_binair_op(bblock_p bp, line_p lnp, token_p tkp1, token_p tkp2)
{
	struct avail av;
	occur_p	ocp;

	av.av_instr = INSTR(lnp);
	av.av_size = avsize(lnp);
	av.av_oleft = tkp1->tk_vn;
	av.av_oright = tkp2->tk_vn;

	ocp = newoccur(tkp1->tk_lfirst, lnp, bp);

	push_avail(av_enter(&av, ocp, BINAIR_OP), tkp1->tk_lfirst);
}

STATIC void push_ternair_op(bblock_p bp, line_p lnp, token_p tkp1,
			    token_p tkp2, token_p tkp3)
{
	struct avail av;
	occur_p	ocp;

	av.av_instr = INSTR(lnp);
	av.av_size = avsize(lnp);
	av.av_ofirst = tkp1->tk_vn;
	av.av_osecond = tkp2->tk_vn;
	av.av_othird = tkp3->tk_vn;

	ocp = newoccur(tkp1->tk_lfirst, lnp, bp);

	push_avail(av_enter(&av, ocp, TERNAIR_OP), tkp1->tk_lfirst);
}

STATIC void push_remainder(bblock_p bp, line_p lnp, token_p tkp1, token_p tkp2)
{
	/* Enter the implicit division tkp1 / tkp2,
	 * then push the remainder tkp1 % tkp2.
	 */
	struct avail av;
	occur_p	ocp;

	assert(INSTR(lnp) == op_rmi || INSTR(lnp) == op_rmu);
	av.av_size = avsize(lnp);
	av.av_oleft = tkp1->tk_vn;
	av.av_oright = tkp2->tk_vn;

	/* Check whether we may convert RMI/RMU to DVI/DVU. */
	if (may_become_dv()) {
		/* The division is DVI in RMI, or DVU in RMU. */
		av.av_instr = (INSTR(lnp) == op_rmi ? op_dvi : op_dvu);

		/* In postfix, a b % becomes a b a b / * -.  We must
		 * keep a and b on the stack, so the first instruction
		 * to eliminate is lnp, not tkp1->l_first.
		 */
		ocp = newoccur(lnp, lnp, bp);
		av_enter(&av, ocp, BINAIR_OP);
	}

	av.av_instr = INSTR(lnp);
	ocp = newoccur(tkp1->tk_lfirst, lnp, bp);
	push_avail(av_enter(&av, ocp, REMAINDER), tkp1->tk_lfirst);
}

STATIC void fiddle_stack(line_p lnp)
{
	/* The instruction in lnp does something to the valuenumber-stack. */

	struct token dummy;
	offset size;

	/* Partly initialize dummy. */
	dummy.tk_lfirst = lnp;

	switch (INSTR(lnp)) {
		default:
			assert(FALSE);
			break;
		case op_lor:
			dummy.tk_vn = newvalnum(); dummy.tk_size = ps;
			Push(&dummy);
			break;
		case op_asp:
			if ((size = off_set(lnp)) > 0) {
				Pop(&dummy, size);
			} else {
				dummy.tk_vn = newvalnum();
				dummy.tk_size = size;
				Push(&dummy);
			}
			break;
		case op_dup:
			Dup(lnp);
			break;
		case op_ass:
		case op_dus:
		case op_exg:
		case op_los:
			/* Don't waste effort. */
			clr_stack();
			break;
		case op_sig:
			Pop(&dummy, (offset) ps);
			break;
		case op_lfr:
			dummy.tk_vn = newvalnum();
			dummy.tk_size = off_set(lnp);
			Push(&dummy);
			break;
		case op_beq:
		case op_bge:
		case op_bgt:
		case op_bne:
		case op_ble:
		case op_blt:
			Pop(&dummy, (offset) ws);
			Pop(&dummy, (offset) ws);
			break;
		case op_bra:
		case op_csa:/* ??? */
		case op_csb:/* ??? */
		case op_gto:/* ??? */
		case op_ret:/* ??? */
		case op_rtt:/* ??? */
			break;
		case op_zeq:
		case op_zge:
		case op_zgt:
		case op_zne:
		case op_zle:
		case op_zlt:
		case op_trp:
			Pop(&dummy, (offset) ws);
			break;
		case op_rck:
			Pop(&dummy, (offset) ps);
			break;
	}
}

STATIC proc_p find_proc(vn)
	valnum vn;
{
	/* Find the procedure-identifier with valuenumber vn. */

	entity_p enp;

	enp = find_entity(vn);

	if (enp != (entity_p) 0 && enp->en_kind == ENPROC)
		return enp->en_pro;

	return (proc_p) 0;
}

STATIC void side_effects(line_p lnp)
{
	/* Lnp contains a cai or cal instruction. We try to find the callee
	 * and see what side-effects it has.
	 */
	struct token tk;
	proc_p pp;

	if (INSTR(lnp) == op_cai) {
		Pop(&tk, (offset) ps);
		pp = find_proc(tk.tk_vn);
	} else {
		assert(INSTR(lnp) == op_cal);
		pp = PROC(lnp);
	}
	if (pp != (proc_p) 0) {
		kill_call(pp);
	} else {
		kill_much();
	}
}

STATIC void hopeless(int instr)
{
	/* The effect of `instr' is too difficult to
	 * compute. We assume worst case behaviour.
	 */
	switch (instr) {
		default:
			assert(FALSE);
			break;
		case op_mon:
		case op_str:
		case op_nop:	/* for volatiles */
			/* We can't even trust "static" entities. */
			kill_all();
			clr_stack();
			break;
		case op_blm:
		case op_bls:
		case op_sts:
			kill_much();
			clr_stack();
			break;
	}
}

void vnm(bblock_p bp)
{
	register line_p lnp;
	register entity_p rep;
	line_p lfirst;
	struct token tk, tk1, tk2, tk3;

	for (lnp = bp->b_start; lnp != (line_p) 0; lnp = lnp->l_next) {

		rep = getentity(lnp, &lfirst);
		switch (instrgroup(lnp)) {
			case SIMPLE_LOAD:	
				push_entity(rep, lfirst);
				break;
			case LOAD_ARRAY:
				put_aar(bp, lnp, lfirst, rep);
				/* Fall through ... */
			case EXPENSIVE_LOAD:
				push_entity(rep, lfirst);
				put_expensive_load(bp, lnp, lfirst, rep);
				break;
			case STORE_DIRECT:
				kill_direct(rep);
				Pop(&tk, rep->en_size);
				rep->en_vn = tk.tk_vn;
				break;
			case STORE_ARRAY:
				put_aar(bp, lnp, lfirst, rep);
				/* Fall through ... */
			case STORE_INDIR:
				kill_indir(rep);
				Pop(&tk, rep->en_size);
				rep->en_vn = tk.tk_vn;
				break;
			case UNAIR_OP:
				Pop(&tk1, op11size(lnp));
				push_unair_op(bp, lnp, &tk1);
				break;
			case BINAIR_OP:
				Pop(&tk2, op22size(lnp));
				Pop(&tk1, op12size(lnp));
				push_binair_op(bp, lnp, &tk1, &tk2);
				break;
			case TERNAIR_OP:
				Pop(&tk3, op33size(lnp));
				Pop(&tk2, op23size(lnp));
				Pop(&tk1, op13size(lnp));
				push_ternair_op(bp, lnp, &tk1, &tk2, &tk3);
				break;
			case REMAINDER:
				Pop(&tk2, op22size(lnp));
				Pop(&tk1, op12size(lnp));
				push_remainder(bp, lnp, &tk1, &tk2);
				break;
			case KILL_ENTITY:
				kill_direct(rep);
				break;
			case SIDE_EFFECTS:
				side_effects(lnp);
				break;
			case FIDDLE_STACK:
				fiddle_stack(lnp);
				break;
			case IGNORE:
				break;
			case HOPELESS:
				hopeless(INSTR(lnp));
				break;
			case BBLOCK_END:
				fiddle_stack(lnp);
				break;
			default:
				assert(FALSE);
				break;
		}
	}
}
