#ifndef lint
static char rcsid[] = "$Header$";
#endif lint

#include "const.h"
#include "assert.h"

bool	bytes_reversed = FALSE;
bool	words_reversed = FALSE;

/*
 * Determine the byte/word order in shorts/longs, assuming the size of a short
 * is 2 chars, and the size of a long is 4 chars. Not all theoretical
 * possibilities are tested; only bytes reversed and/or words reversed.
 */
determine_ordering()
{
	short	s;
	long	l;
	register char	*cp;
	register short	*sp;

	cp = (char *)&s;
	cp[0] = 0x01; cp[1] = 0x02;
	if (s != 0x01 + (0x02 << 8))
		bytes_reversed = TRUE;
	sp = (short *)&l;
	sp[0] = 0x0001; sp[1] = 0x0002;
	if (l != 0x0001 + (0x0002L << 16))
		words_reversed = TRUE;
}

/*
 * `Format' is a string of digits indicating how many bytes must be taken
 * from `buf' to form an integer of some type. E.g. if the digit is '2', two
 * bytes are taken to form a short.
 */
swap(buf, format)
	register char	*buf;
	register char	*format;
{
	register char	savebyte;

	while (*format) {
		switch (*format++) {
		case '1':
			buf += 1;
			break;
		case '2':
			if (bytes_reversed) {
				savebyte = buf[0];
				buf[0] = buf[1];
				buf[1] = savebyte;
			}
			buf += 2;
			break;
		case '4':
			/*
			 * Written out to save recursive calls.
			 */
			if (bytes_reversed && words_reversed) {
				savebyte = buf[0];
				buf[0] = buf[3];
				buf[3] = savebyte;
				savebyte = buf[1];
				buf[1] = buf[2];
				buf[2] = savebyte;
			} else if (bytes_reversed) {
				savebyte = buf[0];
				buf[0] = buf[1];
				buf[1] = savebyte;
				savebyte = buf[2];
				buf[2] = buf[3];
				buf[3] = savebyte;
			} else if (words_reversed) {
				savebyte = buf[0];
				buf[0] = buf[2];
				buf[2] = savebyte;
				savebyte = buf[1];
				buf[1] = buf[3];
				buf[3] = savebyte;
			}
			buf += 4;
			break;
		default:
			assert(FALSE);
			break;
		}
	}
}
