/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID0 "$Id$"

/*
 * Mostek 6500 options.
 */
#define	THREE_PASS	/* Distinguish short and long branches. */
#define	LISTING		/* Enable listing facilities. */
#define RELOCATION	/* generate relocation info */
#define DEBUG 0

#undef ALIGNWORD
#define ALIGNWORD 2
#undef ALIGNSECT
#define ALIGNSECT 2

