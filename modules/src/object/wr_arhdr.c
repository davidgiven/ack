#include <arch.h>
#include "object.h"

wr_arhdr(fd, arhdr)
	register struct ar_hdr	*arhdr;
{
#if ! (BYTES_REVERSED || WORDS_REVERSED)
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
		put4(arhdr->ar_date,c); c += 4;
		*c++ = arhdr->ar_uid;
		*c++ = arhdr->ar_gid;
		put2(arhdr->ar_mode,c); c += 2;
		put4(arhdr->ar_size,c);
		wr_bytes(fd, buf, (long) AR_TOTAL);
	}
#if ! (BYTES_REVERSED || WORDS_REVERSED)
	else	wr_bytes(fd, (char *) arhdr, (long) AR_TOTAL);
#endif
}
