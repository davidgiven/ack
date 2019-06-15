/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include <stdlib.h>
#include <stdio.h>
#include "obj.h"

#define MININT      (1 << (sizeof(int) * 8 - 1))
#define MAXCHUNK    (~MININT)   /* Highest count we write(2).   */
/* Notice that MAXCHUNK itself might be too large with some compilers.
   You have to put it in an int!
*/

static const int maxchunk = MAXCHUNK;

extern void wr_fatal(void);


/*
 * Just write "cnt" bytes to file-descriptor "fd".
 */
void wr_bytes(FILE* fd, const char *buf, long cnt)
{

    size_t written_bytes;
    while (cnt)
    {
        int n = cnt >= maxchunk ? maxchunk : cnt;

        written_bytes = fwrite(buf, 1, n, fd);
        if (written_bytes != (size_t)n)
        {
            wr_fatal();
        }
        buf += n;
        cnt -= n;
    }
}
