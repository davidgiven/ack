/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>
#include <string.h>
#include <em_mnem.h>
#include <em_spec.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/aux.h"
#include "../share/cset.h"
#include "../share/lset.h"
#include "cs.h"
#include "cs_aux.h"
#include "cs_debug.h"
#include "cs_avail.h"
#include "cs_partit.h"

STATIC cset	addr_modes;
STATIC cset	cheaps;
STATIC cset	forbidden;
STATIC cset	sli_counts;
STATIC short	LX_threshold;
STATIC short	AR_limit;

STATIC void get_instrs(FILE *f, cset *s_p)
{
	/* Read a set of integers from inputfile f into *s_p.
	 * Such a set must be delimited by a negative number.
	 */
	int instr;

	fscanf(f, "%d", &instr);
	while (instr >= 0) {
		Cadd((Celem_t) instr, s_p);
		fscanf(f, "%d", &instr);
	}
}

STATIC void choose_cset(FILE *f, cset *s_p, int max)
{
	/* Read two compact sets of integers from inputfile f.
	 * Choose the first if we optimize with respect to time,
	 * the second if we optimize with respect to space, as
	 * indicated by time_space_ratio.
	 */
	cset cs1, cs2; /* Two dummy sets. */

	*s_p = Cempty_set((short) max);

	cs1 = Cempty_set((short) max);
	get_instrs(f, &cs1);
	cs2 = Cempty_set((short) max);
	get_instrs(f, &cs2);

	Ccopy_set(time_space_ratio >= 50 ? cs1 : cs2, s_p);

	Cdeleteset(cs1); Cdeleteset(cs2);
}

void cs_machinit(void *vp)
{
	FILE *f = vp;
	char s[100];
	int time, space;

	/* Find piece that is relevant for this phase. */
	do {
		while (getc(f) != '\n');
		fscanf(f, "%s", s);
	} while (strcmp(s, "%%CS"));

	/* Choose a set of instructions which must only be eliminated
	 * if they are at the root of another expression.
	 */
	choose_cset(f, &addr_modes, sp_lmnem);

	/* Choose a set of cheap instructions; i.e. instructions that
	 * are cheaper than a move to save the result of such an
	 * instruction.
	 */
	choose_cset(f, &cheaps, sp_lmnem);

	/* Read how many lexical levels back an LXL/LXA instruction
	 * must at least look before it will be eliminated.
	 */
	fscanf(f, "%d %d", &time, &space);
	LX_threshold = time_space_ratio >= 50 ? time : space;

	/* Read what the size of an array-element may be,
	 * before we think that it is to big to replace
	 * a LAR/SAR of it by AAR LOI/STI <size>.
	 */
	fscanf(f, "%d", &space);
	AR_limit = space;

	/* Read for what counts we must not eliminate an SLI instruction
	 * when it is part of an array-index computation.
	 */
	choose_cset(f, &sli_counts, 8 * ws);

	/* Read a set of instructions which we do not want to eliminate.
	 * Note: only instructions need be given that may in principle
	 * be eliminated, but for which better code can be generated
	 * when they stay, and with which is not dealt in the common
	 * decision routines.
	 */
	choose_cset(f, &forbidden, sp_lmnem);
}

STATIC bool sli_no_eliminate(line_p lnp)
{
	/* Return whether the SLI-instruction in lnp is part of
	 * an array-index computation, and should not be eliminated.
	 */
	offset cst;

	return	lnp->l_prev != (line_p) 0 && INSTR(lnp->l_prev) == op_loc &&
		lnp->l_next != (line_p) 0 && INSTR(lnp->l_next) == op_ads &&
		((cst = off_set(lnp->l_prev)), cst == (Celem_t) cst) &&
		Cis_elem((Celem_t) cst, sli_counts)
		;
}

STATIC bool gains(avail_p avp)
{
	/* Return whether we can gain something, when we eliminate
	 * an expression such as in avp. We just glue together some
	 * heuristics with some user-supplied stuff.
	 */
	if (Cis_elem(avp->av_instr & BMASK, forbidden))
		return FALSE;

	if (avp->av_instr == (byte) op_lxa || avp->av_instr == (byte) op_lxl)
		return off_set(avp->av_found) >= LX_threshold;

	if (avp->av_instr == (byte) op_sli || avp->av_instr == (byte) op_slu)
		return ! sli_no_eliminate(avp->av_found);

	if (avp->av_instr == (byte) op_ads &&
	    avp->av_found->l_prev && 
	    ( INSTR(avp->av_found->l_prev) == op_sli ||
	      INSTR(avp->av_found->l_prev) == op_slu))
		return ! sli_no_eliminate(avp->av_found->l_prev);

	if (Cis_elem(avp->av_instr & BMASK, addr_modes))
		return instrgroup(avp->av_found->l_prev) != SIMPLE_LOAD;

	if (Cis_elem(avp->av_instr & BMASK, cheaps))
		return avp->av_saveloc != (entity_p) 0;

	return TRUE;
}

STATIC bool okay_lines(avail_p avp, occur_p ocp)
{
	register line_p lnp, next;
	offset sz;

	for (lnp = ocp->oc_lfirst; lnp != (line_p) 0; lnp = next) {
		next = lnp != ocp->oc_llast ? lnp->l_next : (line_p) 0;

		if (INSTR(lnp) < sp_fmnem || INSTR(lnp) > sp_lmnem)
			return FALSE;
		if (!stack_group(INSTR(lnp))) {
			/* Check for SAR-instruction. */
			if (INSTR(lnp) != op_sar || next != (line_p) 0)
				return FALSE;
		}
	}
	/* All lines in this occurrence can in principle be eliminated;
	 * no stores, messages, calls etc.
	 * We now check whether it is desirable to treat a LAR or a SAR
	 * as an AAR LOI/STI. This depends on the size of the array-elements.
	 */
	if (INSTR(ocp->oc_llast) == op_lar || INSTR(ocp->oc_llast) == op_sar) {
		sz = array_elemsize(avp->av_othird);
		if (sz == UNKNOWN_SIZE) return FALSE;
		if (avp->av_instr == (byte) op_aar && time_space_ratio < 50) {
			return sz <= AR_limit;
		}
	}
	return TRUE;
}

bool desirable(avail_p avp)
{
	register Lindex i, next;

	if (!gains(avp)) {
		OUTTRACE("no gain", 0);
		SHOWAVAIL(avp);
		return FALSE;
	}

	/* Walk through the occurrences to see whether it is okay to
	 * eliminate them. If not, remove them from the set.
	 */
	for (i = Lfirst(avp->av_occurs); i != (Lindex) 0; i = next) {
		next = Lnext(i, avp->av_occurs);

		if (!okay_lines(avp, occ_elem(i))) {
			OUTTRACE("may not eliminate", 0);
#			ifdef TRACE
				SHOWOCCUR(occ_elem(i));
#			endif
			oldoccur(occ_elem(i));
			Lremove(Lelem(i), &avp->av_occurs);
		}
	}

	return Lnrelems(avp->av_occurs) > 0;
}
