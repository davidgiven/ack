/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <em_mnem.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/utils.h"
#include "../share/map.h"
#include "cs.h"
#include "cs_aux.h"
#include "cs_debug.h"
#include "cs_avail.h"
#include "cs_entity.h"
#include "cs_kill.h"

STATIC valnum base_valno(entity_p enp)
{
	/* Return the value number of the (base) address of an indirectly
	 * accessed entity.
	 */
	switch (enp->en_kind) {
		default:
			assert(FALSE);
			break;
		case ENINDIR:
			return enp->en_ind;
		case ENOFFSETTED:
			return enp->en_base;
		case ENARRELEM:
			return enp->en_arbase;
	}
	/* NOTREACHED */
}

STATIC entity_p find_base(valnum vn)
{
	/* Vn is the valuenumber of the (base) address of an indirectly
	 * accessed entity. Return the entity that holds this address
	 * recursively.
	 */
	register Lindex i;
	register avail_p ravp;

	for (i = Lfirst(entities); i != (Lindex) 0; i = Lnext(i, entities)) {
		register entity_p renp = en_elem(i);

		if (renp->en_vn == vn) {
			switch (renp->en_kind) {
				case ENAEXTERNAL:
				case ENALOCAL:
				case ENALOCBASE:
				case ENAARGBASE:
					return renp;
				case ENAOFFSETTED:
					return find_base(renp->en_base);
			}
		}
	}

	/* We couldn't find it among the entities.
	 * Let's try the available expressions.
	 */
	for (ravp = avails; ravp != (avail_p) 0; ravp = ravp->av_before) {
		if (ravp->av_result == vn) {
			if (ravp->av_instr == (byte) op_aar)
				return find_base(ravp->av_ofirst);
			if (ravp->av_instr == (byte) op_ads)
				return find_base(ravp->av_oleft);
		}
	}

	/* Bad luck. */
	return (entity_p) 0;
}

STATIC bool obj_overlap(obj_p op1, obj_p op2)
{
	/* Op1 and op2 point to two objects in the same datablock.
	 * Obj_overlap returns whether these objects might overlap.
	 */
	obj_p tmp;

	if (op1->o_off > op2->o_off) {
		/* Exchange them. */
		tmp = op1; op1 = op2; op2 = tmp;
	}
	return	op1->o_size == UNKNOWN_SIZE ||
		op1->o_off + op1->o_size > op2->o_off;
}

#define same_datablock(o1, o2)	((o1)->o_dblock == (o2)->o_dblock)

STATIC bool addr_local(entity_p enp)
{
	/* Is enp the address of a stack item. */

	if (enp == (entity_p) 0) return FALSE;

	return	enp->en_kind == ENALOCAL || enp->en_kind == ENALOCBASE ||
		enp->en_kind == ENAARGBASE;
}

STATIC bool addr_external(entity_p enp)
{
	/* Is enp the address of an external. */

	return enp != (entity_p) 0 && enp->en_kind == ENAEXTERNAL;
}

STATIC void kill_external(obj_p obp, int indir)
{
	/* A store is done via the object in obp. If this store is direct
	 * we kill directly accessed entities in the same data block only
	 * if they overlap with obp, otherwise we kill everything in the
	 * data block. Indirectly accessed entities of which it can not be
	 * proven taht they are not in the same data block, are killed in
	 * both cases.
	 */
	register Lindex i;

	OUTTRACE("kill external", 0);
	for (i = Lfirst(entities); i != (Lindex) 0; i = Lnext(i, entities)) {
		entity_p enp = en_elem(i);
		entity_p base;

		switch (enp->en_kind) {
			case ENEXTERNAL:
				if (!same_datablock(enp->en_ext, obp))
					break;
				if (!indir && !obj_overlap(enp->en_ext, obp))
					break;
				OUTTRACE("kill %d", enp->en_vn);
				enp->en_vn = newvalnum();
				break;
			case ENINDIR:
			case ENOFFSETTED:
			case ENARRELEM:
				/* We spare its value number if we are sure
				 * that its (base) address points into the
				 * stack or into another data block.
				 */
				base = find_base(base_valno(enp));
				if (addr_local(base))
					break;
				if (addr_external(base) &&
				    !same_datablock(base->en_ext, obp)
				   )
					break;
				OUTTRACE("kill %d", enp->en_vn);
				enp->en_vn = newvalnum();
				break;
		}
	}
}

STATIC bool loc_overlap(entity_p enp1, entity_p enp2)
{
	/* Enp1 and enp2 point to two locals. Loc_overlap returns whether
	 * they overlap.
	 */
	entity_p tmp;

	assert(enp1->en_kind == ENLOCAL && enp2->en_kind == ENLOCAL);

	if (enp1->en_loc > enp2->en_loc) {
		/* Exchange them. */
		tmp = enp1; enp1 = enp2; enp2 = tmp;
	}
	if (enp1->en_loc < 0 && enp2->en_loc >= 0)
		return	FALSE; /* Locals and parameters do not overlap. */
	else	return	enp1->en_size == UNKNOWN_SIZE ||
			enp1->en_loc + enp1->en_size > enp2->en_loc;
}

STATIC void kill_local(entity_p enp, bool indir)
{
	/* This time a store is done into an ENLOCAL. */

	register Lindex i;

	OUTTRACE("kill local", 0);
	for (i = Lfirst(entities); i != (Lindex) 0; i = Lnext(i, entities)) {
		entity_p rep = en_elem(i);
		entity_p base;

		switch (rep->en_kind) {
			case ENLOCAL:
				if (indir) {
					/* Kill locals that might be stored into
					 * via a pointer. Note: enp not used.
					 */
					if (!is_regvar(rep->en_loc)) {
						OUTTRACE("kill %d", rep->en_vn);
						rep->en_vn = newvalnum();
					}
				} else if (loc_overlap(rep, enp)) {
					/* Only kill overlapping locals. */
					OUTTRACE("kill %d", rep->en_vn);
					rep->en_vn = newvalnum();
				}
				break;
			case ENINDIR:
			case ENOFFSETTED:
			case ENARRELEM:
				if (!is_regvar(enp->en_loc)) {
					base = find_base(base_valno(rep));
					if (!addr_external(base)) {
						OUTTRACE("kill %d", rep->en_vn);
						rep->en_vn = newvalnum();
					}
				}
				break;
			case ENALOCBASE:
			case ENAARGBASE:
				if (enp->en_loc == 0 && rep->en_levels >= 1) {
					rep->en_vn = newvalnum();
				}
				break;
		}
	}
}

STATIC void kill_sim(void)
{
	/* A store is done into the ENIGNMASK. */

	register Lindex i;

	OUTTRACE("kill sim", 0);
	for (i = Lfirst(entities); i != (Lindex) 0; i = Lnext(i, entities)) {
		register entity_p rep = en_elem(i);

		if (rep->en_kind == ENIGNMASK) {
			OUTTRACE("kill %d", rep->en_vn);
			rep->en_vn = newvalnum();
			return; /* There is only one ignoremask. */
		}
	}
}

void kill_direct(entity_p enp)
{
	/* A store will be done into enp. We must forget the values of all the
	 * entities this one may overlap with.
	 */
	switch (enp->en_kind) {
		default:
			assert(FALSE);
			break;
		case ENEXTERNAL:
			kill_external(enp->en_ext, FALSE);
			break;
		case ENLOCAL:
			kill_local(enp, FALSE);
			break;
		case ENIGNMASK:
			kill_sim();
			break;
	}
}

void kill_indir(entity_p enp)
{
	/* An indirect store is done, in an ENINDIR,
	 * an ENOFFSETTED or an ENARRELEM.
	 */
	entity_p p;

	/* If we can find the (base) address of this entity, then we can spare
	 * the entities that are provably not pointed to by the address.
	 * We will also make use of the MES 3 pseudo's, generated by
	 * the front-end. When a MES 3 is generated for a local, this local
	 * will not be referenced indirectly.
	 */
	if ((p = find_base(base_valno(enp))) == (entity_p) 0) {
		kill_much(); /* Kill all entities without registermessage. */
	} else {
		switch (p->en_kind) {
			case ENAEXTERNAL:
				/* An indirect store into global data. */
				kill_external(p->en_ext, TRUE);
				break;
			case ENALOCAL:
			case ENALOCBASE:
			case ENAARGBASE:
				/* An indirect store into stack data.  */
				kill_local(p, TRUE);
				break;
		}
	}
}

extern void kill_much(void)
{
	/* Kills all killable entities,
	 * except the locals for which a registermessage was generated.
	 */
	register Lindex i;

	OUTTRACE("kill much", 0);
	for (i = Lfirst(entities); i != (Lindex) 0; i = Lnext(i, entities)) {
		register entity_p rep = en_elem(i);

		if (rep->en_static) continue;
		if (rep->en_kind == ENLOCAL && is_regvar(rep->en_loc)) continue;
		OUTTRACE("kill %d", rep->en_vn);
		rep->en_vn = newvalnum();
	}
}

STATIC bool bad_procflags(proc_p pp)
{
	/* Return whether the flags about the procedure in pp indicate
	 * that we have little information about it. It might be that
	 * we haven't seen the text of pp, or that we have seen that pp
	 * calls a procedure which we haven't seen the text of.
	 */
	return !(pp->p_flags1 & PF_BODYSEEN) || (pp->p_flags1 & PF_CALUNKNOWN);
}

STATIC void kill_globset(cset s)
{
	/* S is a set of global variables that might be changed.
	 * We act as if a direct store is done into each of them.
	 */
	register Cindex i;

	OUTTRACE("kill globset", 0);
	for (i = Cfirst(s); i != (Cindex) 0; i = Cnext(i,s)) {
		kill_external(omap[Celem(i)], FALSE);
	}
}

void kill_call(proc_p pp)
{
	/* Kill everything that might be destroyed by calling
	 * the procedure in pp.
	 */
	if (bad_procflags(pp)) {
		/* We don't know enough about this procedure. */
		kill_much();
	} else if (pp->p_change->c_flags & CF_INDIR) {
		/* The procedure does an indirect store. */
		kill_much();
	} else {
		/* Procedure might affect global data. */
		kill_globset(pp->p_change->c_ext);
	}
}

void kill_all(void)
{
	/* Kills all entities. */

	register Lindex i;

	OUTTRACE("kill all entities", 0);
	for (i = Lfirst(entities); i != (Lindex) i; i = Lnext(i, entities)) {
		entity_p enp = en_elem(i);

		OUTTRACE("kill %d", enp->en_vn);
		enp->en_vn = newvalnum();
	}
}
