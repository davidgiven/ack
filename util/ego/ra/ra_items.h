/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ I T E M S . H
 */

extern short item_type();	/* (line_p l)
				 * Determine the type of item (constant,local
				 * variable etc.) accessed by l.
				 */
extern bool is_item();		/* (line_p l)
				 * See if l accesses an item
				 */
extern item_p item_of();	/* (offset off;item_p items)
				 * Determine the descriptor of the item
				 * accessed by l; return 0 if not found
				 */
extern fill_item();		/* (item_p item;line_p l)
				 * Compute the type and obj/off attributes
				 * of the item accessed by l and put them
				 * in the given item descriptor.
				 */
extern bool same_item();	/* (item_p a,b)
				 * See if a and b are the same items.
				 */
extern build_itemlist();	/* (proc_p p;item_p items[]; int *nrinstr_out)
				 * Determine all items accessed by procedure p
				 * and put them in the items lists. All items
				 * of type T must be put in list items[T].
				 * Also determine the number of instructions
				 * of p.
				 */
