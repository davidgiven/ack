/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include <stdio.h>

#if ! defined(CHAR_UNSIGNED)
#define CHAR_UNSIGNED 0
#endif

#define Xchar(ch)   (uint32_t)(uint8_t)(ch)


#define uget2(c)    (Xchar((c)[0]) | (Xchar((c)[1])<<8))
#define get2(c)     ((int16_t) uget2(c))
#define put2(i, c)  (((c)[0] = i), ((c)[1] = i>>8))

#define uget4(c)    (uget2(c) | (uget2((c)+2)<<16))
#define get4(c)     ((int32_t) uget4(c))
#define put4(i, c)  (put2(i, c), put2((i)>>16, (c)+2))

#define SECTCNT 3   /* number of sections with own output buffer */
#if BIGMACHINE
#define WBUFSIZ (8*BUFSIZ)
#else
#define WBUFSIZ BUFSIZ
#endif

struct fil {
    int cnt;
    char    *pnow;
    char    *pbegin;
    long    currpos;
    FILE*   fd;
    char    pbuf[WBUFSIZ];
};

extern struct fil __parts[];

#define PARTEMIT    0
#define PARTRELO    (PARTEMIT+SECTCNT)
#define PARTNAME    (PARTRELO+1)
#define PARTCHAR    (PARTNAME+1)
#ifdef SYMDBUG
#define PARTDBUG    (PARTCHAR+1)
#else
#define PARTDBUG    (PARTCHAR+0)
#endif
#define NPARTS      (PARTDBUG + 1)

#define getsect(s)      (PARTEMIT+((s)>=(SECTCNT-1)?(SECTCNT-1):(s)))
