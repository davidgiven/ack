/*
 * flushbuf.c - flush a buffer
 */
/* $Header$ */

#include	<stdio.h>
#include	<stdlib.h>
#include	"loc_incl.h"

int write(int d, const char *buf, int nbytes);
int isatty(int d);

int
_flushbuf(int c, FILE * stream)
{
	if (fileno(stream) < 0) return EOF;
	if (!io_testflag(stream, _IONBF)) {
		if (!stream->_buf) {
			if (stream == stdout && isatty(fileno(stdout))) {
				if (!(stream->_buf =
					    (unsigned char *) malloc(BUFSIZ))) {
					stream->_flags |= _IONBF;
				}
				else {
					stream->_flags |= _IOLBF;
					stream->_bufsiz = BUFSIZ;
					stream->_count = -1;
				}
			}
			else {
				if (!(stream->_buf =
					    (unsigned char *) malloc(BUFSIZ))) {
					stream->_flags |= _IONBF;
				}
				else {
					stream->_flags |= _IOMYBUF;
					stream->_bufsiz = BUFSIZ;
					stream->_count = BUFSIZ - 1;
				}
			}
			stream->_ptr = stream->_buf;
		}
	}

	if (io_testflag(stream, _IONBF)) {
		char c1 = c;

		stream->_count = 0;
		if (write(fileno(stream), &c1, 1) != 1) {
			stream->_flags |= _IOERR;
			return EOF;
		}
		return c;
	}
	else if (io_testflag(stream, _IOLBF)) {
		*stream->_ptr++ = c;
		if (c == '\n' || stream->_count == -stream->_bufsiz) {
			if (write(fileno(stream), (char *)stream->_buf,
					-stream->_count) != -stream->_count) {
				stream->_flags |= _IOERR;
				return EOF;
			}
			else {
				stream->_ptr  = stream->_buf;
				stream->_count = 0;
			}
		}
	}
	else {
		int count = stream->_ptr - stream->_buf;

		stream->_count = stream->_bufsiz - 1;
		stream->_ptr = stream->_buf + 1;

		if (count > 0) {
			if (write(fileno(stream), (char *)stream->_buf, count)
			    != count) {
				*(stream->_buf) = c;
				stream->_flags |= _IOERR;
				return EOF;
			}
		}
		*(stream->_buf) = c;
	}
	return c;
}

void
_cleanup(void)		/* fflush((FILE *)NULL) ??? */
{
	register int i;

	for( i = 0; i < FOPEN_MAX; i++ )
		if (_iotable[i])
			fclose(_iotable[i]);
}
