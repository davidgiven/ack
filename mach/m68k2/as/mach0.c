/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)mach0.c	1.5 */
/*
 * Motorola 68000/68010 options
 */

#define	THREE_PASS		/* branch and offset optimization */
#define	BYTES_REVERSED		/* high order byte has lowest address */
#define	WORDS_REVERSED		/* high order word has lowest address */
#define	LISTING			/* enable listing facilities */
#define RELOCATION		/* generate relocatable code */
#define DEBUG 0

#define Xfit(f)		if (!(f)) Xnofit();

#undef	ADDR_T
#define	ADDR_T		long

#undef	ALIGNWORD
#define	ALIGNWORD	2
#undef	ALIGNSECT
#define	ALIGNSECT	2

#undef	VALWIDTH
#define	VALWIDTH	8
