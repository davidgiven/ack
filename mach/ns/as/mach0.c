#define RCSID0 "$Header$"

/*
 * NS 16032 options
 */
#define	THREE_PASS	/* branch and offset optimization */
#define	LISTING		/* enable listing facilities */

#undef	valu_t
#define valu_t		long
#undef	addr_t
#define	addr_t		long
#undef	ALIGNSECT
#define ALIGNSECT 	2

#undef	VALWIDTH
#define	VALWIDTH	8

#define RELOCATION

#define UNUSED		/* Assemble the 'unused' instructions like bfalse */
