/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 1 _ A N A L . H
 */

extern apriori();      /* (proc_p proctab)
			* For every procedure, see if we can determine
	 		* from the information provided by the previous
	 		* phases of the optimizer that it cannot or should not
	 		* be expanded in line. This will reduce the length
	 		* of the call list.
	 		*/
extern anal_proc();    /* (proc_p p, FILE *cf, *cff)
			* Analyse a procedure. See which formal parameters
			* it uses and which procedures it calls.
			* cf and ccf are the call-file and the call-count file.
			*/
