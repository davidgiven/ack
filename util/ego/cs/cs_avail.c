/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* M O D U L E   F O R   A C C E S S S I N G   T H E   L I S T
 *
 * O F   A V A I L A B L E   E X P R E S S I O N S
 */

#include <em_mnem.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/aux.h"
#include "../share/lset.h"
#include "../share/global.h"
#include "cs.h"
#include "cs_aux.h"
#include "cs_debug.h"
#include "cs_alloc.h"
#include "cs_getent.h"

avail_p avails; /* The list of available expressions. */

STATIC bool commutative(int instr)
{
	/* Is instr a commutative operator? */

	switch (instr) {
		case op_adf: case op_adi: case op_adu: case op_and:
		case op_cms: case op_ior: case op_mlf: case op_mli:
		case op_mlu:
			return TRUE;
		default:
			return FALSE;
	}
}

STATIC bool same_avail(byte kind, avail_p avp1, avail_p avp2)
{
	/* Two expressions are the same if they have the same operator,
	 * the same size, and their operand(s) have the same value. 
	 * Only if the operator is commutative, the order of the operands
	 * does not matter.
	 */
	if (avp1->av_instr != avp2->av_instr) return FALSE;
	if (avp1->av_size != avp2->av_size) return FALSE;

	switch (kind) {
		default:
			assert(FALSE);
			break;
		case EXPENSIVE_LOAD:
		case UNAIR_OP:
			return	avp1->av_operand == avp2->av_operand;
		case BINAIR_OP:
		case REMAINDER:
			if (commutative(avp1->av_instr & BMASK))
				return	avp1->av_oleft == avp2->av_oleft &&
					avp1->av_oright == avp2->av_oright
					||
					avp1->av_oleft == avp2->av_oright &&
					avp1->av_oright == avp2->av_oleft
					;
			else
				return	avp1->av_oleft == avp2->av_oleft &&
					avp1->av_oright == avp2->av_oright;
		case TERNAIR_OP:
			return	avp1->av_ofirst == avp2->av_ofirst &&
				avp1->av_osecond == avp2->av_osecond &&
				avp1->av_othird == avp2->av_othird;
	}
	/* NOTREACHED */
}

STATIC void check_local(avail_p avp)
{
	/* Check if the local in which the result of avp was stored,
	 * still holds this result. Update if not.
	 */
	if (avp->av_saveloc == (entity_p) 0) return; /* Nothing to check. */

	if (avp->av_saveloc->en_vn != avp->av_result) {
		OUTTRACE("save local changed value", 0);
		avp->av_saveloc = (entity_p) 0;
	}
}

STATIC entity_p result_local(offset size, line_p l)
{
	/* If the result of an expression of size bytes is stored into a
	 * local for which a registermessage was generated, return a pointer
	 * to this local.
	 */
	line_p dummy;
	entity_p enp;

	if (l == (line_p) 0)
		return (entity_p) 0;

	if (INSTR(l)==op_stl && size==ws || INSTR(l)==op_sdl && size==2*ws) {
		enp = getentity(l, &dummy);
		if (is_regvar(enp->en_loc)) {
			OUTTRACE("save local found, %ld(LB)", enp->en_loc);
			return enp;
		}
	}

	return (entity_p) 0;
}

STATIC void copy_avail(int kind, avail_p src, avail_p dst)
{
	/* Copy some attributes from src to dst. */

	dst->av_instr = src->av_instr;
	dst->av_size = src->av_size;

	switch (kind) {
		default:
			assert(FALSE);
			break;
		case EXPENSIVE_LOAD:
		case UNAIR_OP:
			dst->av_operand = src->av_operand;
			break;
		case BINAIR_OP:
		case REMAINDER:
			dst->av_oleft = src->av_oleft;
			dst->av_oright = src->av_oright;
			break;
		case TERNAIR_OP:
			dst->av_ofirst = src->av_ofirst;
			dst->av_osecond = src->av_osecond;
			dst->av_othird = src->av_othird;
			break;
	}
}

avail_p av_enter(avail_p avp, occur_p ocp, int kind)
{
	/* Put the available expression avp in the list,
	 * if it is not already there.
	 * Add ocp to the set of occurrences of this expression.
	 */
	register avail_p ravp;
	line_p last = ocp->oc_llast;

	for (ravp = avails; ravp != (avail_p) 0; ravp = ravp->av_before) {
		if (same_avail(kind, ravp, avp)) { /* It was there. */
			Ladd(ocp, &ravp->av_occurs);
			/* Can we still use the local in which
			 * the result was stored?
			 */
			check_local(ravp);
			return ravp;
		}
	}
	/* A new available axpression. */
	ravp = newavail();

	/* Remember local, if any, that holds result. */
	if (avp->av_instr != (byte) INSTR(last)) {
		/* Only possible when instr is the implicit AAR in 
		 * a LAR or SAR, or the implicit DVI in an RMI, or
		 * DVU in RMU.
		 */
		ravp->av_saveloc = (entity_p) 0;
	} else {
		ravp->av_saveloc = result_local(avp->av_size, last->l_next);
	}
	ravp->av_found = last;
	ravp->av_result = kind == EXPENSIVE_LOAD? avp->av_operand: newvalnum();
	copy_avail(kind, avp, ravp);
	oldoccur(ocp);
	ravp->av_before = avails;
	avails = ravp;
	return ravp;
}

void clr_avails(void)
{
	/* Throw away the information about the available expressions. */

	register avail_p ravp, next;
	register Lindex i;
	register lset s;

	for (ravp = avails; ravp != (avail_p) 0; ravp = next) {
		next = ravp->av_before;

		s = ravp->av_occurs;
		for (i = Lfirst(s); i != (Lindex) 0; i = Lnext(i, s)) {
			oldoccur(occ_elem(i));
		}
		Ldeleteset(s);
		oldavail(ravp);
	}
	avails = (avail_p) 0;
}
