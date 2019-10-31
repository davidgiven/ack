/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ L I F E T I M E . H
 */


void build_lifetimes(item_p items[]);
				/*
				 * compute the it_lives attribute of every
				 * item; this is a list of intervals
				 *  during which the item is live,
				 * i.e. its current value may be used.
				 */
