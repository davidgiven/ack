/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S R _ A U X . H  */


extern bool is_loopconst();	/* (line_p l; lset vars)
				 * See if l is a loop-constant. vars is the
				 * set of variables changed in the loop.
				 */
extern bool is_caddress();	/* (line_p l)
				 * See if l loads a loop-invariant entity of
				 * size pointer-size.
				 */
extern int elemsize();		/* (line_p l)
				 * l is an instruction that loads an array
				 * descriptor. Try to determine the size
				 * of the array elements.
				 */
extern	   concatenate();	/* (line_p list1,list2)
				 * Append list2 to the end of list1
				 */
#define is_const(l)	(INSTR(l) == op_loc)
