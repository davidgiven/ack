/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* F U N C T I O N S   F O R   A C C E S S I N G   T H E   S E T
 *
 * O F   E N T I T I E S
 */

#include "../share/types.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/debug.h"
#include "cs.h"
#include "cs_alloc.h"
#include "cs_aux.h"

lset entities; /* Our pseudo symbol-table. */

entity_p find_entity(valnum vn)
{
	/* Try to find the entity with valuenumber vn. */

	register Lindex i; 

	for (i = Lfirst(entities); i != (Lindex) 0; i = Lnext(i, entities)) {
		if (en_elem(i)->en_vn == vn)
			return en_elem(i);
	}

	return (entity_p) 0;
}

STATIC bool same_entity(entity_p enp1, entity_p enp2)
{
	if (enp1->en_kind != enp2->en_kind) return FALSE;
	if (enp1->en_size != enp2->en_size) return FALSE;
	if (enp1->en_size == UNKNOWN_SIZE) return FALSE;

	switch (enp1->en_kind) {
		case ENCONST:
			return	enp1->en_val == enp2->en_val;
		case ENLOCAL:
		case ENALOCAL:
			return	enp1->en_loc == enp2->en_loc;
		case ENEXTERNAL:
		case ENAEXTERNAL:
			return	enp1->en_ext == enp2->en_ext;
		case ENINDIR:
			return	enp1->en_ind == enp2->en_ind;
		case ENOFFSETTED:
		case ENAOFFSETTED:
			return	enp1->en_base == enp2->en_base &&
				enp1->en_off == enp2->en_off;
		case ENALOCBASE:
		case ENAARGBASE:
			return	enp1->en_levels == enp2->en_levels;
		case ENPROC:
			return	enp1->en_pro == enp2->en_pro;
		case ENARRELEM:
			return	enp1->en_arbase == enp2->en_arbase &&
				enp1->en_index == enp2->en_index &&
				enp1->en_adesc == enp2->en_adesc;
		default:
			return	TRUE;
	}
}

STATIC void copy_entity(entity_p src, entity_p dst)
{
	dst->en_static = src->en_static;
	dst->en_kind = src->en_kind;
	dst->en_size = src->en_size;

	switch (src->en_kind) {
		case ENCONST:
			dst->en_val = src->en_val;
			break;
		case ENLOCAL:
		case ENALOCAL:
			dst->en_loc = src->en_loc;
			break;
		case ENEXTERNAL:
		case ENAEXTERNAL:
			dst->en_ext = src->en_ext;
			break;
		case ENINDIR:
			dst->en_ind = src->en_ind;
			break;
		case ENOFFSETTED:
		case ENAOFFSETTED:
			dst->en_base = src->en_base;
			dst->en_off = src->en_off;
			break;
		case ENALOCBASE:
		case ENAARGBASE:
			dst->en_levels = src->en_levels;
			break;
		case ENPROC:
			dst->en_pro = src->en_pro;
			break;
		case ENARRELEM:
			dst->en_arbase = src->en_arbase;
			dst->en_index = src->en_index;
			dst->en_adesc = src->en_adesc;
			break;
	}
}

entity_p en_enter(entity_p enp)
{
	/* Put the entity in enp in the entity set, if it is not already there.
	 * Return pointer to stored entity.
	 */
	register Lindex i;
	register entity_p new;

	for (i = Lfirst(entities); i != (Lindex) 0; i = Lnext(i, entities)) {
		if (same_entity(en_elem(i), enp))
			return en_elem(i);
	}
	/* A new entity. */
	new = newentity();
	new->en_vn = newvalnum();
	copy_entity(enp, new);
	Ladd(new, &entities);

	return new;
}

void clr_entities(void)
{
	/* Throw away all pseudo-symboltable information. */

	register Lindex i;

	for (i = Lfirst(entities); i != (Lindex) 0; i = Lnext(i, entities)) {
		oldentity(en_elem(i));
	}
	Ldeleteset(entities);
	entities = Lempty_set();
}
