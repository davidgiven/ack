/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <arch.h>
#include "object.h"

wr_arhdr(fd, arhdr)
	register struct ar_hdr	*arhdr;
{
#if WORDS_REVERSED && !BYTES_REVERSED
	if (sizeof (struct ar_hdr) != AR_TOTAL)
#endif
	{	
		char buf[AR_TOTAL];
		register char *c = buf;
		register char *p = arhdr->ar_name;
		register int i = 14;

		while (i--) {
			*c++ = *p++;
		}
		put2(arhdr->ar_date>>16,c); c += 2;
		put2(arhdr->ar_date,c); c += 2;
		*c++ = arhdr->ar_uid;
		*c++ = arhdr->ar_gid;
		put2(arhdr->ar_mode,c); c += 2;
		put2(arhdr->ar_size>>16,c); c += 2;
		put2(arhdr->ar_size,c);
		wr_bytes(fd, buf, (long) AR_TOTAL);
	}
#if WORDS_REVERSED && !BYTES_REVERSED
	else	wr_bytes(fd, (char *) arhdr, (long) AR_TOTAL);
#endif
}
