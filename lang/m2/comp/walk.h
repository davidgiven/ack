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

#define	WalkNode(xnd, xlab)	(*WalkTable[(xnd)->nd_class])((xnd), (xlab))

extern label	text_label;
extern label	data_label;

#ifndef SQUEEZE
#define c_loc(x)	C_loc((arith) (x))
#define c_lae_dlb(x)	C_lae_dlb(x,(arith) 0)
#endif
