/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef _CTYPE_H
#define _CTYPE_H

/*  File   : ctypes.h
    Author : Richard A. O'Keefe.
    Updated: 26 April 1984
    Purpose: Reimplement the UNIX ctype(3) library.

    isaneol(c) means that c is a line terminating character.
    isalnum, ispunct, isspace, and isaneol are defined on the
    range -1..127, i.e. on ASCII U {EOF}, while all the other
    macros are defined for any integer.

    isodigit(c) checks for Octal digits.
    isxdigit(c) checkx for heXadecimal digits.
*/

#define isdigit(c)	((unsigned)((c)-'0') < 10)
#define islower(c)	((unsigned)((c)-'a') < 26)
#define isupper(c)	((unsigned)((c)-'A') < 26)
#define isprint(c)	((unsigned)((c)-' ') < 95)
#define iscntrl(c)	((unsigned)((c)-' ') >= 95)
#define isascii(c)	((unsigned)(c) < 128)
#define isalpha(c)	((unsigned)(((c)|32)-'a') < 26)

extern	char	_c2type[];

#define isalnum(c)	(_c2type[(c)+1] < 36)
#define ispunct(c)	(_c2type[(c)+1] == 36)
#define isspace(c)	(_c2type[(c)+1] > 37)
#define isaneol(c)	(_c2type[(c)+1] > 38)

#define	isxdigit(c)	(_c2type[(c)+1] < 16)
#define isodigit(c)	((unsigned)((c)-'0') < 8)

/*  The following "conversion" macros have been in some versions of UNIX
    but are not in all.  tocntrl is new.  The original motivation for ^?
    being a name for DEL was that (x)^64 mapped A..Z to ^A..^Z and also
    ? to DEL.  The trouble is that this trick doesn't work for lower case
    letters.  The version given here is not mine.  I wish it was.  It has
    the nice property that DEL is mapped to itself (so does EOF).
    tolower(c) and toupper(c) are only defined when isalpha(c).
*/
#define	tolower(c)	((c)|32)
#define toupper(c)	((c)&~32)
#define tocntrl(c)	(((((c)+1)&~96)-1)&127)
#define	toascii(c)	((c)&127)

#endif /* _CTYPE_H */
