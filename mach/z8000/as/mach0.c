/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID0 "$Header$"

/*
** Zilog z8000 machine dependent options
*/
#define THREE_PASS
#define BYTES_REVERSED
#define WORDS_REVERSED
#define LISTING
#define ASLD
#undef ALIGNSECT
#define ALIGNSECT	2
#undef valu_t
#define valu_t long
#undef ADDR_T
#define ADDR_T long
