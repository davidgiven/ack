/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID0 "$Id$"

/*
 * PDP 11 machine dependent options
 */

#define THREE_PASS
#define WORDS_REVERSED
#define LISTING
#define RELOCATION
#undef ISALPHA
#define ISALPHA(c)	(isalpha(c) || c == '_' || c == '.' || c == '~')
#undef ISALNUM
#define ISALNUM(c)	(isalnum(c) || c == '_' || c == '.' || c == '~')

#undef ALIGNWORD
#define ALIGNWORD 2
#undef ALIGNSECT
#define ALIGNSECT 2
