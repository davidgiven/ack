/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
extern occur_p	newoccur();	/* (line_p l1, l2; bblock_p b)
				 * Returns a pointer to a new struct occur
				 * and initializes it.
				 */

extern		oldoccur();	/* (occur_p ocp)
				 * Release the struct occur ocp points to.
				 */

extern avail_p	newavail();	/* ()
				 * Return a pointer to a new struct avail.
				 */

extern		oldavail();	/* (avail_p avp)
				 * Release the struct avail avp points to.
				 */

extern entity_p	newentity();	/* ()
				 * Return a pointer to a new struct entity.
				 */

extern		oldentity();	/* (entity_p enp)
				 * Release the struct entity enp points to.
				 */
