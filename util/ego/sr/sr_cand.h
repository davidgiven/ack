/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S T R E N G T H   R E D U C T I O N
 *
 *  S R _ C A N D . H
 */


extern	candidates();		/* (loop_p lp; lset *iv_cand, *vars)
				 * Find candidate induction variables,
				 * i.e. local variables that are assigned
				 * a value precisely once within the loop,
				 * within a strong block. Also find the
				 * local variables that are changed within
				 * the loop, but that are not a candidate.
				 */
