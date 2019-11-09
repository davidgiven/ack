/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 1 _ F O R M A L . C
 */

void formal(proc_p p, bblock_p b, offset off, int type, int usage);
			/*
			 * Analyze a reference to a parameter of p.
			 * The type denotes its size (single,double,
			 * pointer).
			 */
