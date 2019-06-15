/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.

 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdlib.h>
#include <stdio.h>
#include "out.h"
#include "arch.h"
#include "object.h"
#include "obj.h"

extern void rd_fatal(void);


int rd_arhdr(FILE* fd, register struct ar_hdr *arhdr)
{
    char buf[AR_TOTAL];
    register char *c = buf;
    register char *p = arhdr->ar_name;
    register size_t i;

    i = fread(c, 1, AR_TOTAL, fd);
    if (i == 0) return 0;
    if (i != AR_TOTAL) {
        rd_fatal();
    }
    i = AR_NAME_MAX;
    while (i--) {
        *p++ = *c++;
    }
    arhdr->ar_date = ((long) get2(c)) << 16; c += 2;
    arhdr->ar_date |= ((long) get2(c)) & 0xffff; c += 2;
    arhdr->ar_uid = *c++;
    arhdr->ar_gid = *c++;
    arhdr->ar_mode = get2(c); c += 2;
    arhdr->ar_size = (long) get2(c) << 16; c += 2;
    arhdr->ar_size |= (long) get2(c) & 0xffff;
    return 1;
}
