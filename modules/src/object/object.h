/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "byte_order.h"

#if CHAR_UNSIGNED
#define Xchar(ch)	(ch)
#else
#define Xchar(ch)	((ch) & 0377)
#endif

#if BYTES_REVERSED
#define uget2(c)	(Xchar((c)[0]) | ((unsigned) Xchar((c)[1]) << 8))
#define Xput2(i, c)	(((c)[0] = (i)), ((c)[1] = (i) >> 8))
#define put2(i, c)	{ register int j = (i); Xput2(j, c); }
#else
#define uget2(c)	(* ((unsigned short *) (c)))
#define Xput2(i, c)	(* ((short *) (c)) = (i))
#define put2(i, c)	Xput2(i, c)
#endif

#define get2(c)		((short) uget2(c))

#if WORDS_REVERSED || BYTES_REVERSED
#define get4(c)		(uget2(c) | ((long) uget2((c)+2) << 16))
#define put4(l, c)	{ register long x=(l); \
			  Xput2((int)x,c); \
			  Xput2((int)(x>>16),(c)+2); \
			}
#else
#define get4(c)		(* ((long *) (c)))
#define put4(l, c)	(* ((long *) (c)) = (l))
#endif
