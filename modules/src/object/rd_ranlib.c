#include <ranlib.h>
#include "object.h"

rd_ranlib(fd, ran, cnt)
	register struct ranlib	*ran;
	register long	cnt;
{
	rd_bytes(fd, (char *) ran, cnt * SZ_RAN);
#if ! (BYTES_REVERSED || WORDS_REVERSED)
	if (sizeof (struct ranlib) != SZ_RAN)
#endif
	{
		register char *c = (char *) ran;

		while (cnt--) {
			ran->ran_off = get4(c); c += 4;
			ran->ran_pos = get4(c); c += 4;
			ran++;
		}
	}
}
