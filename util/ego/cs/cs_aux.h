/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
extern offset	array_elemsize();	/* (valnum vm)
					 * Returns the size of array-elements,
					 * if vn is the valuenumber of the
					 * address of an array-descriptor.
					 */

extern occur_p	occ_elem();		/* (Lindex i)
					 * Returns a pointer to the occurrence
					 * of which i is an index in a set.
					 */

extern entity_p	en_elem();		/* (Lindex i)
					 * Returns a pointer to the entity
					 * of which i is an index in a set.
					 */

extern valnum	newvalnum();		/* ()
					 * Returns a completely new
					 * value number.
					 */

extern		start_valnum();		/* ()
					 * Restart value numbering.
					 */

