/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID1 "$Id$"

/*
 * Zilog Z80 dependent C declarations.
 */

#define	B		0
#define	C		1
#define	D		2
#define	E		3
#define	H		4
#define	L		5
#define	F		6
#define	A		7
#define	I		8
#define	R		9
#define	BC		0
#define	DE		1
#define	HL		2
#define	SP		3
#define	AF		4
#define	AF2		5
#define	IX		6
#define	IY		7

#define	low3(z)		((z) & 07)
#define	fit3(z)		(low3(z) == (z))

extern expr_t	exp_ind;
#ifndef ASLD
extern int	rel_ind;
#endif
