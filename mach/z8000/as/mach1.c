/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID1 "$Id$"

/*
** Zilog z8000 machine dependent C declarations
*/
#define	REG	1
#define	IM	2
#define	IR	4
#define	DA	8
#define	X	16
#define	RA	32
#define	BA	64
#define	BX	128

#define	TYPE_11a23	IR | DA | X
#define	TYPE_11b23	REG | IR | DA | X
#define	TYPE_12		REG | IR | DA | X
#define	TYPE_1263	REG | IR | DA | X
#define	TYPE_jp		IR | DA | X
#define	TYPE_21a	REG | IR | DA | X
#define	TYPE_21b	REG
#define	TYPE_2151	REG | IM | IR | DA | X
#define	TYPE_pop	REG | IR | DA | X
#define	TYPE_push	REG | IM | IR | DA | X
#define	TYPE_ldm	IR | DA | X
#define	TYPE_ld		IR | DA | X

#define	DST	0
#define	SRC	1

#define DJNZ_	0xF000
#define JR_	0xE000
#define CALR_	0xD000

#define low3(z)		( ((short) z) & 07 )
#define low4(z)		( ((short) z) & 017 )
#define low5(z)		( ((short) z) & 037 )
#define low7(z)		( ((short) z) & 0177 )
#define low8(z)		( ((short) z) & 0377 )
#define low11(z)	( ((short) z) & 03777 )
#define low12(z)	( ((short) z) & 07777 )
#define low15(z)	( ((short) z) & 077777 )
#define fit3(z)		( low3(z) == (z) )
#define fit4(z)		( low4(z) == (z) )
#define fit5(z)		( low5(z) == (z) )
#define fit7(z)		( low7(z) == (z) )
#define fit8(z)		( low8(z) == (z) )
#define fits8(z)	( low7(z) == (z) || low7(-z-1) == (-z-1) )
#define fits12(z)	( low11(z) == (z) || low11(-z-1) == (-z-1) )
#define fits16(z)	( low15(z) == (z) || low15(-z-1) == (-z-1) )

#define bitset(x,b)	( ((int) x) & ((int) 1<<b) ) /*bit b set in x?*/

typedef struct address	ATYPE;

struct address
{	short	seg;
	short	off;
};

extern expr_t	displ;	/* never more than 16 bits */
extern short	index;	/* never more than 16 bits */
extern short	mode;
#ifndef extern
extern short	modetbl[ 5 ];
#else
short modetbl[5] = { 8, 0, 0, 4, 4 };
	      /* Modes for REG, IM, IR, DA, X.  e.g. modetbl[ 2log REG ]
	      ** is the mode if operand is of type REG.
	      ** No entries for RA, BA and BX are needed.
	      */
#endif
extern short	operand;
extern short	oprtype[ 2 ];
extern expr_t	addr_inf;	/* .val (address) up to 23 bits! */
extern expr_t	immed;

#define regerr()	serror("register error")
