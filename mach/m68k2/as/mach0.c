#define DUK
/* @(#)mach0.c	1.5 */
/*
 * Motorola 68000/68010 options
 */
#undef	BITMAX
#define	BITMAX	8000

#define	THREE_PASS		/* branch and offset optimization */
#define	BYTES_REVERSED		/* high order byte has lowest address */
#define	WORDS_REVERSED		/* high order word has lowest address */
#define	LISTING			/* enable listing facilities */
#define RELOCATION		/* generate relocatable code */

#undef	valu_t
#define	valu_t		long
#undef	addr_t
#define	addr_t		long

#undef	ALIGNWORD
#define	ALIGNWORD	2
#undef	ALIGNSECT
#define	ALIGNSECT	2

#undef	VALWIDTH
#define	VALWIDTH	8

#define NOLD	/* Added by Duk Bekema. */
