/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>
#include <string.h>
#include <em_mnem.h>
#include <em_spec.h>
#include "ego/share/types.h"
#include "ego/share/mach.h"
#include "ego/share/debug.h"
#include "ego/share/global.h"
#include "ego/share/utils.h"
#include "ego/share/cset.h"
#include "ego/share/lset.h"
#include "cs.h"
#include "cs_alloc.h"
#include "cs_aux.h"
#include "cs_debug.h"
#include "cs_avail.h"
#include "cs_partit.h"

static cset addr_modes;
static cset cheaps;
static cset forbidden;
static cset sli_counts;
static short LX_threshold;
static short AR_limit;
static bool RM_to_DV;

static cset get_cset(config_setting_t* policy, const char* name, int max)
{
	/* Copy the named integer array into an optimizer-owned compact set. */
	config_setting_t* values = config_setting_get_member(policy, name);
	cset result = Cempty_set((short)max);
	int i;
	for (i = 0; i < config_setting_length(values); i++)
		Cadd((Celem_t)config_setting_get_int_elem(values, i), &result);
	return result;
}

void cs_machinit(config_t* config)
{
	/* Choose the time or space policy according to time_space_ratio. */
	config_setting_t* policy = mach_setting(
	    config,
	    time_space_ratio >= 50 ? "common_subexpressions.time" : "common_subexpressions.space");

	/* Choose a set of instructions which must only be eliminated
	 * if they are at the root of another expression.
	 */
	addr_modes = get_cset(policy, "addressing_modes", sp_lmnem);

	/* Choose a set of cheap instructions; i.e. instructions that
	 * are cheaper than a move to save the result of such an
	 * instruction.
	 */
	cheaps = get_cset(policy, "cheap_operations", sp_lmnem);

	/* Read how many lexical levels back an LXL/LXA instruction
	 * must at least look before it will be eliminated.
	 */
	LX_threshold = mach_int(policy, "lexical_threshold");

	/* Read what the size of an array-element may be,
	 * before we think that it is too big to replace
	 * a LAR/SAR of it by AAR LOI/STI <size>.
	 */
	AR_limit = mach_int(mach_setting(config, "common_subexpressions"), "indirection_limit");

	/* Read whether to convert a remainder RMI/RMU to a division
	 * DVI/DVU using the formula a % b = a - b * (a / b).
	 */
	RM_to_DV = mach_bool(policy, "convert_remainder_to_division");

	/* Read for what counts we must not eliminate an SLI instruction
	 * when it is part of an array-index computation.
	 */
	sli_counts = get_cset(policy, "excluded_index_shift_counts", 8 * ws);

	/* Read a set of instructions which we do not want to eliminate.
	 * Note: only instructions need be given that may in principle
	 * be eliminated, but for which better code can be generated
	 * when they stay, and with which is not dealt in the common
	 * decision routines.
	 */
	forbidden = get_cset(policy, "forbidden_operations", sp_lmnem);
}

bool may_become_aar(avail_p avp)
{
	/* Check whether it is desirable to treat a LAR or SAR as an
	 * AAR LOI/STI. This depends on the size of the array-elements.
	 */
	offset sz;

	sz = array_elemsize(avp->av_othird);
	if (sz == UNKNOWN_SIZE)
		return false;
	if (time_space_ratio < 50)
		return sz <= AR_limit;
	return true;
}

bool may_become_dv(void)
{
	return RM_to_DV;
}

static bool sli_no_eliminate(line_p lnp)
{
	/* Return whether the SLI-instruction in lnp is part of
	 * an array-index computation, and should not be eliminated.
	 */
	offset cst;

	return lnp->l_prev != (line_p)0 && INSTR(lnp->l_prev) == op_loc && lnp->l_next != (line_p)0
	    && INSTR(lnp->l_next) == op_ads && ((cst = off_set(lnp->l_prev)), cst == (Celem_t)cst)
	    && Cis_elem((Celem_t)cst, sli_counts);
}

static bool gains(avail_p avp)
{
	/* Return whether we can gain something, when we eliminate
	 * an expression such as in avp. We just glue together some
	 * heuristics with some user-supplied stuff.
	 */
	if (Cis_elem(avp->av_instr & BMASK, forbidden))
		return false;

	if (avp->av_instr == (byte)op_lxa || avp->av_instr == (byte)op_lxl)
		return off_set(avp->av_found) >= LX_threshold;

	if (avp->av_instr == (byte)op_sli || avp->av_instr == (byte)op_slu)
		return !sli_no_eliminate(avp->av_found);

	if (avp->av_instr == (byte)op_ads && avp->av_found->l_prev
	    && (INSTR(avp->av_found->l_prev) == op_sli || INSTR(avp->av_found->l_prev) == op_slu))
		return !sli_no_eliminate(avp->av_found->l_prev);

	if (Cis_elem(avp->av_instr & BMASK, addr_modes))
		/* If it is the first instruction of its basic block, its
		 * operand comes from before the block: do not eliminate. */
		return avp->av_found->l_prev != (line_p)0
		    && instrgroup(avp->av_found->l_prev) != SIMPLE_LOAD;

	if (Cis_elem(avp->av_instr & BMASK, cheaps))
		return avp->av_saveloc != (entity_p)0;

	return true;
}

static bool okay_lines(avail_p avp, occur_p ocp)
{
	/* Check whether all lines in this occurrence can in
	 * principle be eliminated; no stores, messages, calls etc.
	 */
	line_p lnp, next;

	for (lnp = ocp->oc_lfirst; lnp != (line_p)0; lnp = next)
	{
		next = lnp != ocp->oc_llast ? lnp->l_next : (line_p)0;

		if (INSTR(lnp) < sp_fmnem || INSTR(lnp) > sp_lmnem)
			return false;
		if (!stack_group(INSTR(lnp)))
		{
			/* Check for SAR-instruction. */
			if (INSTR(lnp) != op_sar || next != (line_p)0)
				return false;
		}
	}
	return true;
}

bool desirable(avail_p avp)
{
	Lindex i, next;

	if (!gains(avp))
	{
		OUTTRACE("no gain", 0);
		SHOWAVAIL(avp);
		return false;
	}

	/* Walk through the occurrences to see whether it is okay to
	 * eliminate them. If not, remove them from the set.
	 */
	for (i = Lfirst(avp->av_occurs); i != (Lindex)0; i = next)
	{
		next = Lnext(i, avp->av_occurs);

		if (!okay_lines(avp, occ_elem(i)))
		{
			OUTTRACE("may not eliminate", 0);
#ifdef TRACE
			SHOWOCCUR(occ_elem(i));
#endif
			oldoccur(occ_elem(i));
			Lremove(Lelem(i), &avp->av_occurs);
		}
	}

	return Lnrelems(avp->av_occurs) > 0;
}
