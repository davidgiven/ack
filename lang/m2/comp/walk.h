/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* P A R S E   T R E E   W A L K E R */

/* $Header$ */

/*	Definition of WalkNode macro
*/

extern int (*WalkTable[])();

#define	WalkNode(xnd, xlab, rch)	(*WalkTable[(xnd)->nd_class])((xnd), (xlab),(rch))

extern label	text_label;
extern label	data_label;

#include "squeeze.h"

#ifndef SQUEEZE
#define c_loc(x)	C_loc((arith) (x))
#define c_lae_dlb(x)	C_lae_dlb(x,(arith) 0)
#define CAL(nm, sz)	(C_cal(nm), C_asp((arith)(sz)))
#define c_bra(x)	C_bra((label) (x))
#endif
