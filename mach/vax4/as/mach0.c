/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID0 "$Header$"

/*
 * VAX-11 machine dependent options
 */

#define THREE_PASS
#define LISTING
#define RELOCATION
#define DEBUG 2			/* as long as we are testing ... */

#undef valu_t
#define valu_t long
#undef addr_t
#define addr_t long

#undef ALIGNWORD
#define ALIGNWORD 4
#undef ALIGNSECT
#define ALIGNSECT 4
