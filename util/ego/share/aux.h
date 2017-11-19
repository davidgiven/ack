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


offset off_set(line_p lnp);	/*
				 * lnp has a SHORT or OFFSET operand. Return
				 * the value of this operand as an offset.
				 */
offset aoff(arg_p list, int n);	/*
				 * Determine the offset field of the
				 * n'th argument in the list (this argument
				 * must have type ARGOFF). Start counting at 0.
				 */
offset tmplocal(proc_p p, offset size);
				/*
				 * Allocate a new local variable in the
				 * stack frame of p.
				 */
line_p int_line(offset off);	/*
				 * Allocate a line struct of type OPSHORT
				 * or OPOFFSET, whichever one fits best.
				 */
line_p reg_mes(offset tmp, short size, int typ, int score);
				/*
				 * Generate a register message with the
				 * given arguments.
				 */
bool dom(bblock_p b1, bblock_p b2);
				/*
				 * See if b1 dominates b2. Note that a
				 * block always * dominates itself.
				 */
bblock_p common_dom(bblock_p a, bblock_p b);
				/*
				 * Find a basic block that dominates a as
				 * well as b; note that a basic block also
				 * dominates itself.
				 */
short add_timespace(short time, short space);
				/*
				 * Add together a time and space, using
				 * the time_space_ratio parameter that
				 * may be set by the user.
				 */
void rm_line(line_p l, bblock_p b);
				/*
				 * Remove line l from b basic block b.
				 */
void appnd_line(line_p l1, line_p l2);
				/*
				 * Put line l1 after l2.
				 */
line_p last_instr(bblock_p b);	/*
				 * Determine the last line of a basic block.
				 */
line_p find_mesreg(offset off);	/*
				 * Find the register message for the local
				 * with the given offset.
				 */
bool is_regvar(offset off);	/*
				 * See if there is a 'register message'
				 * for the local variable with the
				 * given offset.
				 */
offset regv_arg(offset off, int n);
				/*
				 * Fetch the n'th argument of the
				 * register message of the local with
				 * the given offset.
				 */
