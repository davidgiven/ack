/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*  S H A R E D   F I L E
 *
 *  I N I T _ G L O B L S
 *
 */

#include "types.h"
#include "debug.h"
#include "global.h"
#include "alloc.h"
#include "map.h"


extern short nrglobals;

init_globals()
{
	/* Assign a 'global variable number (o_globnr) to
	 * every global variable for which we want to
	 * maintain ud-info. We do not maintain ud-info
	 * for a global variable if:
	 *  - it is part of a ROM data block (so it will never be changed)
	 *  - it's size is not known
	 *  - it overlaps another variable (e.g. LOE X+2 ; LDE X)
	 */

	dblock_p d;
	obj_p obj, prev;
	short nr = 1;
	offset ill_zone, x;

	for (d = fdblock; d != (dblock_p) 0; d = d->d_next) {
		ill_zone = (offset) 0;
		for (obj = d->d_objlist; obj != (obj_p) 0; obj = obj->o_next) {
			if (d->d_pseudo == DROM ||
			    obj->o_size == UNKNOWN_SIZE) {
				obj->o_globnr = 0; /* var. not considered */
				continue;
			}
			if (obj->o_off < ill_zone) {
				obj->o_globnr = 0; /* var. not considered */
				if (prev != (obj_p) 0 && prev->o_globnr != 0) {
					prev->o_globnr = 0;
					nr--;
				}
			} else {
				obj->o_globnr = nr++;
			}
			if ((x = obj->o_off + obj->o_size) > ill_zone) {
				ill_zone = x;
			}
			prev = obj;
		}
	}
	nrglobals = nr -1;
}
