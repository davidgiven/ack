/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ I N T E R V A L . H
 */


extern interv_p cons_interval();/* (short t_start,t_stop)
				 * construct an interval
				 */
extern add_interval();		/* (short t1,t2; interv_p *list)
				 * Add interval (t1,t2) to the list of
				 * intervals (which is an in-out parameter!).
				 */
extern interv_p loop_lifetime();/* (loop_p lp)
				 * Determine the timespan of the loop,
				 * expressed as a list of intervals.
				 */
extern interv_p proc_lifetime();/* (proc_p p)
				 * Determine the timespan of a procedure,
				 * expressed as an interval.
				 */
extern interv_p  intersect(); 	/* (interv_p list1,list2)
				 * Intersect two lifetimes, each denoted
				 * by a list of intervals.
				 */
extern bool not_disjoint();	/* (interv_p list1,list2)
				 * See if list1 and list2 do overlap somewhere.
				 */
extern bool contains();		/* (short t;interv_p timespan)
				 * See if t is part of the timespan.
				 */
extern interv_p copy_timespan();/* (interv_p list)
				 * Make a copy of the timespan.
				 */
