#include <arch.h>
#include "object.h"

rd_arhdr(fd, arhdr)
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

		rd_bytes(fd, c, (long) AR_TOTAL);
		while (i--) {
			*p++ = *c++;
		}
		arhdr->ar_date = get4(c); c += 4;
		arhdr->ar_uid = *c++;
		arhdr->ar_gid = *c++;
		arhdr->ar_mode = get2(c); c += 2;
		arhdr->ar_size = get4(c);
	}
#if ! (BYTES_REVERSED || WORDS_REVERSED)
	else	rd_bytes(fd, (char *) arhdr, (long) AR_TOTAL);
#endif
}
