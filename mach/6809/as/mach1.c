/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID1 "$Id$"

/*
 * Motorola 6809 C declarations
 */

extern int dpvalue INIT(-1);	/* for setdp pseudo-op */

#ifndef extern
extern int regbit[];
#else
int regbit[] = {	/* push-pull coding */
	6,	/* D */
	0x10,	/* X */
	0x20,	/* Y */
	0x40,	/* U */
	0x40,	/* S */
	0x80,	/* PC */
	-1, -1,
	2,	/* A */
	4,	/* B */
	1,	/* CC */
	8,	/* DP */
};
#endif

/*
 * Motorola 6809 register names
 */

#define D	0
#define X	1
#define Y	2
#define U	3
#define S	4
#define PC	5
#define A	8
#define B	9
#define CC	10
#define DP	11
