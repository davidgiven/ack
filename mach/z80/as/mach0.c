/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID0 "$Id$"

/*
 * Zilog Z80 options
 */
#define	THREE_PASS		/* distinguish short and long branches */
#define	LISTING			/* enable listing facilities */
#define RELOCATION

#undef ISALNUM
#define ISALNUM(c)	(isalnum(c) || c == '_' || c == '.')
