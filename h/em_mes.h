/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * mnemonics for the message numbers in EM
 */

#define	ms_err	 0	/* Compilation error occurred, ignore rest of module */
#define	ms_opt	 1	/* Disable optimization please                       */
#define	ms_emx	 2	/* Wordsize and pointersize assumed                  */
#define	ms_reg	 3	/* Hint for possible register usage from frontend    */
#define	ms_src	 4	/* Number of source lines in this module             */
#define	ms_flt	 5	/* Floating point used                               */
#define	ms_com	 6	/* Comment to be retained in compact code            */
#define	ms_ret	 7	/* Reserved                                          */
#define ms_ext	 8	/* List of exported symbols from this library module */
#define ms_par	 9	/* Number of bytes of parameters accessed            */
#define ms_ego	10	/* Hint from EM Global Optimizer                     */
#define ms_gto	11	/* Dangerous procedure, uses nonlocal goto	     */
#define ms_stb	12	/* symbol table entry (for debugger)		     */
#define ms_std	13	/* symbol table entry (for debugger) referring to dot */
#define ms_tes  14      /* Size of the top element at a certain label	     */

/*
 * for details about ms_reg, see em_reg.h
 * for details about ms_ego, see em_ego.h
 */
