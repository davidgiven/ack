/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <arch.h>
#include "object.h"

int
rd_arhdr(fd, arhdr)
	register struct ar_hdr	*arhdr;
{
#if WORDS_REVERSED && ! BYTES_REVERSED
	if (sizeof (struct ar_hdr) != AR_TOTAL)
#endif
	{	
		char buf[AR_TOTAL];
		register char *c = buf;
		register char *p = arhdr->ar_name;
		register int i;

		i = read(fd, c, AR_TOTAL);
		if (i == 0) return 0;
		if (i < 0 || i != AR_TOTAL) {
			rd_fatal();
		}
		i = 14;
		while (i--) {
			*p++ = *c++;
		}
		arhdr->ar_date = get2(c) << 16; c += 2;
		arhdr->ar_date |= get2(c) & 0xffff; c += 2;
		arhdr->ar_uid = *c++;
		arhdr->ar_gid = *c++;
		arhdr->ar_mode = get2(c); c += 2;
		arhdr->ar_size = get2(c) << 16; c += 2;
		arhdr->ar_size |= get2(c) & 0xffff;
	}
#if WORDS_REVERSED && !BYTES_REVERSED
	else	{
		register int i;
		i = read(fd, (char *) arhdr, AR_TOTAL);
		if (i == 0) return 0;
		if (i < 0 || i != AR_TOTAL) rd_fatal();
	}
#endif
	return 1;
}
