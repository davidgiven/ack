/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID0 "$Id$"

/*
 * NS 16032 options
 */
#define	THREE_PASS	/* branch and offset optimization */
#define	LISTING		/* enable listing facilities */

#undef	ADDR_T
#define	ADDR_T		long
#undef	ALIGNSECT
#define ALIGNSECT 	2

#undef	VALWIDTH
#define	VALWIDTH	8

#define RELOCATION

#define UNUSED		/* Assemble the 'unused' instructions like bfalse */
