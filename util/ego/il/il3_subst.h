/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 3 _ S U B S T . H
 */

extern line_p scan_to_cal();	/* (line_p lines; short n)
				 * Find the n-th cal instruction.
				 */
extern substitute();	/* (FILE *lf;call_p c; line_ pcal,firstline)
			 * Perform in line substitution of the call described
	 		 * by c. The EM text of the called routine is fetched
	 		 * and modified, the calling sequence is changed,
	 		 * the modified routine is put at the place of the call
	 		 * and all global information (proctable etc.) is kept
	 		 * up to date.
	 		 */
