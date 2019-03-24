/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include <stdlib.h>
#include <stdio.h>
#include "obj.h"

#define MININT      (1 << (sizeof(int) * 8 - 1))
#define MAXCHUNK    (~MININT)   /* Highest count we read(2).    */
/* Unfortunately, MAXCHUNK is too large with some  compilers. Put it in
   an int!
*/

static int maxchunk = MAXCHUNK;

extern void rd_fatal(void);

/*
 * We don't have to worry about byte order here.
 * Just read "cnt" bytes from file-descriptor "fd".
 */
void
rd_bytes(FILE* fd, register char *string, register long cnt)
{
    size_t read_bytes;
    while (cnt) {
        register size_t n = cnt >= maxchunk ? maxchunk : cnt;

        read_bytes = fread(string, 1, n, fd);
        if (read_bytes != n)
            rd_fatal();
        string += n;
        cnt -= n;
    }
}
