/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <em_reg.h>
#include <em_mnem.h>
#include "../share/types.h"
#include "../share/alloc.h"
#include "../share/lset.h"
#include "../share/utils.h"
#include "../share/global.h"
#include "../share/debug.h"
#include "cs.h"
#include "cs_avail.h"
#include "cs_alloc.h"
#include "cs_aux.h"
#include "cs_debug.h"
#include "cs_profit.h"
#include "cs_partit.h"
#include "cs_debug.h"

STATIC void dlink(line_p l1, line_p l2)
{
	/* Doubly link the lines in l1 and l2. */

	if (l1 != (line_p) 0)
		l1->l_next = l2;
	if (l2 != (line_p) 0)
		l2->l_prev = l1;
}

STATIC void remove_lines(line_p first, line_p last)
{
	/* Throw away the lines between and including first and last.
	 * Don't worry about any pointers; they (must) have been taken care of.
	 */
	register line_p lnp, next;

	last->l_next = (line_p) 0; /* Delimit the list. */
	for (lnp = first; lnp != (line_p) 0; lnp = next) {
		next = lnp->l_next;
		oldline(lnp);
	}
}

STATIC bool contained(occur_p ocp1, occur_p ocp2)
{
	/* Determine whether ocp1 is contained within ocp2. */

	register line_p lnp, next;

	for (lnp = ocp2->oc_lfirst; lnp != (line_p) 0; lnp = next) {
		next = lnp != ocp2->oc_llast ? lnp->l_next : (line_p) 0;

		if (lnp == ocp1->oc_llast) return TRUE;
	}
	return FALSE;
}

STATIC void delete(occur_p ocp, avail_p start)
{
	/* Delete all occurrences that are contained within ocp.
	 * They must have been entered in the list before start:
	 * if an expression is contained with an other, its operator line
	 * appears before the operator line of the other because EM-expressions
	 * are postfix.
	 */
	register avail_p ravp;
	register Lindex	i, next;

	for (ravp = start; ravp != (avail_p) 0; ravp = ravp->av_before) {
		for (i = Lfirst(ravp->av_occurs); i != (Lindex) 0; i = next) {
			next = Lnext(i, ravp->av_occurs);

			if (contained(occ_elem(i), ocp)) {
				OUTTRACE("delete contained occurrence", 0);
#				ifdef TRACE
					SHOWOCCUR(occ_elem(i));
#				endif
				oldoccur(occ_elem(i));
				Lremove(Lelem(i), &ravp->av_occurs);
			}
		}
	}
}

STATIC void complete_aar(line_p lnp, int instr, valnum descr_vn)
{
	/* Lnp is an instruction that loads the address of an array-element.
	 * Instr tells us what effect we should achieve; load (instr is op_lar)
	 * or store (instr is op_sar) this array-element. Descr_vn is the
	 * valuenumber of the address of the descriptor of this array.
	 * We append a loi or sti of the correct number of bytes.
	 */
	register line_p lindir;

	lindir = int_line(array_elemsize(descr_vn));
	lindir->l_instr = instr == op_lar ? op_loi : op_sti;
	dlink(lindir, lnp->l_next);
	dlink(lnp, lindir);
}

STATIC void complete_dv_as_rm(line_p lnp, avail_p avp, bool first)
{
	/* Complete a / b as a % b = a - b * (a / b). For the first
	 * occurrence, lnp must stack q, where q = a / b. We prepend a
	 * DUP to change postfix a b / into a b a b /, then append a
	 * MLI/MLU and SBI/SBU to make a b a b / * -.
	 *
	 * For later occurences, lnp must stack a b q.  We append the
	 * MLI/MLU and SBI/SBU.
	 */
	line_p dv, dup, ml, sb;
	offset size;
	bool s;

	size = avp->av_size;
	s = (avp->av_instr == (byte) op_dvi);
	assert(s || avp->av_instr == (byte) op_dvu);
	if (first) {
		/* Prepend our DUP to avp->av_found, to get before the
		 * DVI if lnp points to the LOL in DVI STL LOL.
		 */
		dup = int_line(2 * size);
		dup->l_instr = op_dup;
		dv = avp->av_found;
		dlink(dv->l_prev, dup);
		dlink(dup, dv);
	}
	ml = int_line(size);
	sb = int_line(size);
	ml->l_instr = (s ? op_mli : op_mlu);
	sb->l_instr = (s ? op_sbi : op_sbu);
	dlink(sb, lnp->l_next);
	dlink(ml, sb);
	dlink(lnp, ml);
}

STATIC void replace(occur_p ocp, offset tmp, avail_p avp)
{
	/* Replace the lines in the occurrence in ocp by a load of the
	 * temporary with offset tmp.
	 */
	avail_p ravp;
	line_p lol, first, last;
	int instr;

	assert(avp->av_size == ws || avp->av_size == 2*ws);

	first = ocp->oc_lfirst; last = ocp->oc_llast;

	lol = int_line(tmp);
	lol->l_instr = avp->av_size == ws ? op_lol : op_ldl;
	dlink(lol, last->l_next);

	if (first->l_prev == (line_p) 0) ocp->oc_belongs->b_start = lol;
	dlink(first->l_prev, lol);

	instr = INSTR(last);
	switch (avp->av_instr & 0377) {
		case op_aar:
			/* There may actually be a LAR or a SAR
			 * instruction; in that case we have to
			 * complete the array-instruction.
			 */
			if (instr != op_aar)
				complete_aar(lol, instr, avp->av_othird);
			break;
		case op_dvi:
			if (instr == op_rmi)
				complete_dv_as_rm(lol, avp, FALSE);
			break;
		case op_dvu:
			if (instr == op_rmu)
				complete_dv_as_rm(lol, avp, FALSE);
			break;
	}

	/* Some occurrence rocp of an expression before avp might have
	 * rocp->oc_lfirst == first.  If so, then we must set
	 * rocp->oc_lfirst = lol before we throw away first.
	 *
	 * This is almost not possible, but it can happen in code with
	 * expr1 LOI expr2 STI expr2 LOI, where the STI causes both
	 * LOIs to have the same value number.  Then the first LOI
	 * might come before the first expr2, so we might replace
	 * expr2 before we replace expr2 LOI.  Then the occurrence of
	 * expr2 LOI must not point to the eliminated lines of expr2.
	 */
	for (ravp = avp->av_before; ravp != (avail_p) 0;
	     ravp = ravp->av_before) {
		/* We only check LOI expressions. */
		if (ravp->av_instr == op_loi) {
			occur_p rocp;
			Lindex i;

			for (i = Lfirst(ravp->av_occurs); i != (Lindex) 0;
			     i = Lnext(i, ravp->av_occurs)) {
				rocp = occ_elem(i);
				if (rocp->oc_lfirst == first)
					rocp->oc_lfirst = lol;
			}
		}
	}

	/* Throw away the by now useless lines. */
	remove_lines(first, last);
}

STATIC void append(avail_p avp, offset tmp)
{
	/* Avp->av_found points to a line with an operator in it. This 
	 * routine emits a sequence of instructions that saves the result
	 * in a local with offset tmp. In most cases we just append
	 * avp->av_found with stl/sdl tmp and lol/ldl tmp depending on
	 * avp->av_size. If however the operator is an aar contained
	 * within a lar or sar, we must first generate the aar.
	 */
	register line_p stl, lol;
	register int instr;

	assert(avp->av_size == ws || avp->av_size == 2*ws);

	stl = int_line(tmp);
	stl->l_instr = avp->av_size == ws ? op_stl : op_sdl;
	lol = int_line(tmp);
	lol->l_instr = avp->av_size == ws ? op_lol : op_ldl;

	dlink(lol, avp->av_found->l_next);
	dlink(stl, lol);
	dlink(avp->av_found, stl);

	instr = INSTR(avp->av_found);
	switch (avp->av_instr & 0377) {
		case op_aar:
			if (instr != op_aar) {
				complete_aar(lol, instr, avp->av_othird);
				avp->av_found->l_instr = op_aar;
			}
			break;
		case op_dvi:
			if (instr == op_rmi) {
				complete_dv_as_rm(lol, avp, TRUE);
				avp->av_found->l_instr = op_dvi;
			}
			break;
		case op_dvu:
			if (instr == op_rmu) {
				complete_dv_as_rm(lol, avp, TRUE);
				avp->av_found->l_instr = op_dvu;
			}
			break;
	}
}

STATIC void set_replace(avail_p avp, offset tmp)
{
	/* Avp->av_occurs is now a set of occurrences, each of which will be
	 * replaced by a reference to a local.
	 * Each time we eliminate an expression, we delete from our
	 * list those expressions that are physically contained in them,
	 * because we cannot eliminate them again.
	 */
	register Lindex i;
	register lset s = avp->av_occurs;

	for (i = Lfirst(s); i != (Lindex) 0; i = Lnext(i, s)) {
		OUTVERBOSE("eliminate duplicate", 0, 0);
		SHOWOCCUR(occ_elem(i));
		Scs++;
		delete(occ_elem(i), avp->av_before);
		replace(occ_elem(i), tmp, avp);
	}
}

STATIC int reg_score(entity_p enp)
{
	/* Enp is a local that will go into a register.
	 * We return its score upto now.
	 */
	assert(is_regvar(enp->en_loc));
	return regv_arg(enp->en_loc, 4);
}

STATIC line_p gen_mesreg(offset off, avail_p avp, proc_p pp)
{
	/* Generate a register message for the local that will hold the
	 * result of the expression in avp, at the appropriate place in
	 * the procedure in pp.
	 */
	register line_p reg;

	reg = reg_mes(off, (short) avp->av_size, regtype(avp->av_instr), 0);
	appnd_line(reg, pp->p_start->b_start);

	return reg;
}

STATIC void change_score(line_p mes, int score)
{
	/* Change the score in the register message in mes to score. */

	register arg_p ap = ARG(mes);

	ap = ap->a_next; /* Offset. */
	ap = ap->a_next; /* Size. */
	ap = ap->a_next; /* Type. */
	ap = ap->a_next; /* Score. */

	ap->a_a.a_offset = score;
}

void eliminate(proc_p pp)
{
	/* Eliminate costly common subexpressions within procedure pp.
	 * We scan the available expressions in - with respect to time found -
	 * reverse order, to find largest first, e.g. `A + B + C' before
	 * `A + B'.
	 * We do not eliminate an expression when the size
	 * is not one of ws or 2*ws, because then we cannot use lol or ldl.
	 * Code is appended to the first occurrence of the expression
	 * to store the result into a local.
	 */
	register avail_p ravp;
	register int score;
	register offset tmp;
	register line_p mes;

	for (ravp = avails; ravp != (avail_p) 0; ravp = ravp->av_before) {

		if (ravp->av_size != ws && ravp->av_size != 2*ws) continue;

		if (ravp->av_saveloc == (entity_p) 0) {
			/* We save it ourselves. */
			score = 2; /* Stl and lol. */
		} else {
			score = reg_score(ravp->av_saveloc);
		}
		if (desirable(ravp)) {
			score += Lnrelems(ravp->av_occurs);
			OUTTRACE("temporary local score %d", score);
			if (ravp->av_saveloc != (entity_p) 0) {
				tmp = ravp->av_saveloc->en_loc;
				mes = find_mesreg(tmp);
				OUTVERBOSE("re-using %ld(LB)", tmp, 0);
			} else {
				tmp = tmplocal(pp,  ravp->av_size);
				mes = gen_mesreg(tmp, ravp, pp);
				append(ravp, tmp);
			}
			change_score(mes, score);
			set_replace(ravp, tmp);
		}
	}
}
