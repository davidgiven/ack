/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  U S E  -  D E F I N I T I O N   A N A L Y S I S
 *
 *  U D . H
 */

#define GEN(b)		(b)->b_extend->bx_ud.bx_gen
#define KILL(b)		(b)->b_extend->bx_ud.bx_kill
#define IN(b)		(b)->b_extend->bx_ud.bx_in
#define OUT(b)		(b)->b_extend->bx_ud.bx_out
#define C_GEN(b)	(b)->b_extend->bx_ud.bx_cgen
#define C_KILL(b)	(b)->b_extend->bx_ud.bx_ckill
#define C_IN(b)		(b)->b_extend->bx_ud.bx_cin
#define C_OUT(b)	(b)->b_extend->bx_ud.bx_cout
#define CHGVARS(b)	(b)->b_extend->bx_ud.bx_chgvars

extern short nrglobals;  /* number of global variables for which
			  * ud-info is maintained.
			  */
extern short nrvars;	 /* total number of variables (global + local)
			  * for which ud-info is maintained.
			  */
