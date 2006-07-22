/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S T R E N G T H   R E D U C T I O N
 *
 * S R _ X F O R M . H
 *
 */



extern line_p move_pointer(); /* (offset tmp; int dir ) */
	/* Generate EM code to load/store a pointer variable
	 * onto/from the stack, depending on dir(ection).
	 * We accept all kinds of pointer sizes.
	 */
extern make_header() ;	/* (loop_p lp) */
	/* Make sure that the loop has a header block, i.e. a block
	 * has the loop entry block as its only successor and
	 * that dominates the loop entry block.
	 * If there is no header yet, create one.
	 */
