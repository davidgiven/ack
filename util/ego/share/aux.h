/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S H A R E D
 *
 *  A U X I L I A R Y   R O U T I N E S
 *
 */


extern offset off_set();	/* (line_p lnp)
				 * lnp has a SHORT or OFFSET operand. Return
				 * the value of this operand as an offset.
				 */
extern	offset aoff();		/* (arg_p list; int n)
				 * Determine the offset field of the
				 * n'th argument in the list (this argument
				 * must have type ARGOFF). Start counting at 0.
				 */
extern	offset tmplocal();	/* (proc_p p, offset size)
				 * Allocate a new local variable in the
				 * stack frame of p.
				 */
line_p int_line();		/* (offset off)
				 * Allocate a line struct of type OPSHORT
				 * or OPOFFSET, whichever one fits best.
	 			 */
extern line_p reg_mes();	/* (offset tmp; short size; int typ,score)
				 * Generate a register message with the
				 * given arguments.
				 */
extern bool dom();		/* (bblock_p b1,b2)
				/* See if b1 dominates b2. Note that a
				 * block always * dominates itself.
				 */
extern bblock_p common_dom();	/* (bblock_p a,b)
				 * find a basic block that dominates a as 
				 * well as b; note that a basic block also 
				 * dominates itself.
				 */
extern short add_timespace(); 	/* (short time,space)
				 * Add together a time and space, using 
				 * the time_space_ratio parameter that 
				 * may be set by the user.
				 */
extern rm_line();		/* ( line_p l; bblock_p b)
				 * Remove line l from b basic block b.
				 */

extern appnd_line();		/* ( line_p l1,l2)
				 * Put line l1 after l2.
				 */
extern line_p last_instr();	/* ( bblock_p b)
				 * Determine the last line of a basic block.
				 */
extern line_p find_mesreg();	/* (offset off)
				 * Find the register message for the local 
				 * with the given offset.
				 */
extern	bool	is_regvar();	/* (offset off)
				 * See if there is a 'register message'
				 * for the local variable with the
				 * given offset.
				 */
extern offset	regv_arg();	/* (offset off; int n)
				 * Fetch the n'th argument of the
				 * register message of the local with
				 * the given offset.
				 */
