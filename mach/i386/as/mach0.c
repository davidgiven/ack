/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID0 "$Id$"

/*
 * INTEL 80386 options
 */
#define	THREE_PASS	/* branch and offset optimization */
#define	LISTING		/* enable listing facilities */
#define RELOCATION	/* generate relocation info */

#undef ADDR_T
#define ADDR_T long

#undef ALIGNWORD
#define ALIGNWORD	4
#undef ALIGNSECT
#define ALIGNSECT	4
