/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  A U X I L I A R Y   R O U T I N E S
 */

#define regv_size(off)		regv_arg(off,2)
					/* Fetch the size argument of the
					 * register message of the local with
					 * the given offset.
					 */
#define regv_type(off)		regv_arg(off,3)
					/* Fetch the type argument of the
					 * register message of the local with
					 * the given offset.
					 */
extern time_p	cons_time();		/* (line_p l; bblock_p b)
					 * Construct a 'time' record with
					 * fields 'l' and 'b'.
					 */
extern short loop_scale(); 		/* (short lev)
					 * Estimate how many times an item
					 * appearing in a loop of nesting
					 * level 'lev' will be used dynamically.
					 */
