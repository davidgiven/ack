/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID1 "$Id$"

/*
 * Intel 8080 register names
 */

#define	B	0
#define	C	1
#define	D	2
#define	E	3
#define	H	4
#define	L	5
#define	M	6
#define	A	7
#define	SP	6
#define	PSW	6

#define	low3(z)		((z) & 07)
#define	fit3(z)		(low3(z) == (z))
