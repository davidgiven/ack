/* $Header$ */
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

STATIC push_entity(enp, lfirst)
	entity_p enp;
	line_p lfirst;
{
	/* Build token and Push it. */

	struct token tk;

	tk.tk_vn = enp->en_vn;
	tk.tk_size = enp->en_size;
	tk.tk_lfirst = lfirst;
	Push(&tk);
}

STATIC put_expensive_load(bp, lnp, lfirst, enp)
	bblock_p bp;
	line_p lnp, lfirst;
	entity_p enp;
{
	struct avail av;
	occur_p	ocp;

	av.av_instr = INSTR(lnp);
	av.av_size = enp->en_size;
	av.av_operand = enp->en_vn;

	ocp = newoccur(lfirst, lnp, bp);

	av_enter(&av, ocp, EXPENSIVE_LOAD);
}

STATIC put_aar(bp, lnp, lfirst, enp)
	bblock_p bp;
	line_p lnp, lfirst;
	entity_p enp;
{
	/* Enp points to an ENARRELEM. We do as if its address was computed. */

	struct avail av;
	occur_p	ocp;

	assert(enp->en_kind == ENARRELEM);
	av.av_instr = op_aar;
	av.av_size = ps;
	av.av_ofirst = enp->en_arbase;
	av.av_osecond = enp->en_index;
	av.av_othird = enp->en_adesc;

	ocp = newoccur(lfirst, lnp, bp);

	av_enter(&av, ocp, TERNAIR_OP);
}

STATIC push_avail(avp, lfirst)
	avail_p avp;
	line_p lfirst;
{
	struct token tk;

	tk.tk_vn = avp->av_result;
	tk.tk_size = avp->av_size;
	tk.tk_lfirst = lfirst;
	Push(&tk);
}

STATIC push_unair_op(bp, lnp, tkp1)
	bblock_p bp;
	line_p lnp;
	token_p tkp1;
{
	struct avail av;
	occur_p	ocp;

	av.av_instr = INSTR(lnp);
	av.av_size = avsize(lnp);
	av.av_operand = tkp1->tk_vn;

	ocp = newoccur(tkp1->tk_lfirst, lnp, bp);

	push_avail(av_enter(&av, ocp, UNAIR_OP), tkp1->tk_lfirst);
}

STATIC push_binair_op(bp, lnp, tkp1, tkp2)
	bblock_p bp;
	line_p lnp;
	token_p tkp1, tkp2;
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

STATIC push_ternair_op(bp, lnp, tkp1, tkp2, tkp3)
	bblock_p bp;
	line_p lnp;
	token_p tkp1, tkp2, tkp3;
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

STATIC fiddle_stack(lnp)
	line_p lnp;
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

STATIC side_effects(lnp)
	line_p lnp;
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

hopeless(instr)
	int instr;
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

vnm(bp)
	bblock_p bp;
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
