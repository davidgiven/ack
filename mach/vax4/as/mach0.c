/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID0 "$Id$"

/*
 * VAX-11 machine dependent options
 */

#define THREE_PASS
#define LISTING
#define RELOCATION

#undef word_t
#define word_t long
#undef ADDR_T
#define ADDR_T long

#undef ALIGNWORD
#define ALIGNWORD 4
#undef ALIGNSECT
#define ALIGNSECT 4
