/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ A L L O C L I S T . H
 */

extern alloc_p build_alloc_list();	/* (proc_p p; short nrloops;
					 *  item_p itemlist)
					 * Build a list of possible allocations
					 * for procedure p. An allocation
					 * essentially is a pair (item,timespan)
					 */
extern build_rivals_graph();	/* (alloc_p alloclist)
				/* See which allocations in the list are
				 * rivals of each other, i.e. there is
				 * some point of time, falling in both
				 * timespans, at which the items of
				 * both allocations are live.
				 */
