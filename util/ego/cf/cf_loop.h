/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  C O N T R O L   F L O W
 *
 *  L O O P   D E T E C T I O N
 */

void loop_detection(proc_p p);	/*
				 * Detect all loops of procedure p.
				 * Every basic block of p is assigned
				 * a set of all loops it is part of.
				 * For every loop we record the number
				 * of blocks it contains, the loop entry
				 * block and its nesting level (0 = outer
				 * loop, 1 = loop within loop etc.).
				 */
