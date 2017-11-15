/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
occur_p newoccur(line_p l1, line_p l2, bblock_p b);
				/*
				 * Returns a pointer to a new struct occur
				 * and initializes it.
				 */

void oldoccur(occur_p ocp);	/*
				 * Release the struct occur ocp points to.
				 */

avail_p	newavail(void);		/*
				 * Return a pointer to a new struct avail.
				 */

void oldavail(avail_p avp);	/*
				 * Release the struct avail avp points to.
				 */

entity_p newentity(void);	/*
				 * Return a pointer to a new struct entity.
				 */

void ldentity(entity_p enp);	/*
				 * Release the struct entity enp points to.
				 */
