/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/aux.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "cs.h"
#include "cs_entity.h"

offset array_elemsize(valnum vn)
{
	/* Vn is the valuenumber of an entity that points to
	 * an array-descriptor. The third element of this descriptor holds
	 * the size of the array-elements.
	 * IF we can find this entity, AND IF we can find the descriptor AND IF
	 * this descriptor is located in ROM, then we return the size.
	 */
	entity_p enp;

	enp = find_entity(vn);

	if (enp == (entity_p) 0)
		return UNKNOWN_SIZE;

	if (enp->en_kind != ENAEXTERNAL)
		return UNKNOWN_SIZE;

	if (enp->en_ext->o_dblock->d_pseudo != DROM)
		return UNKNOWN_SIZE;

	return aoff(enp->en_ext->o_dblock->d_values, 2);
}

occur_p occ_elem(Lindex i)
{
	return (occur_p) Lelem(i);
}

entity_p en_elem(Lindex i)
{
	return (entity_p) Lelem(i);
}

/* The value numbers associated with each distinct value
 * start at 1.
 */

STATIC valnum val_no;

valnum newvalnum(void)
{
	/* Return a completely new value number. */

	return ++val_no;
}

void start_valnum(void)
{
	/* Restart value numbering. */

	val_no = 0;
}
