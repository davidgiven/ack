/*
 * setbuf.c - control buffering of a stream
 */
/* $Header$ */

#include	<stdio.h>
#include	<stdlib.h>
#include	"loc_incl.h"

int
setvbuf(register FILE *stream, char *buf, int mode, size_t size)
{
	int retval = 0;

	if (mode != _IOFBF && mode != _IOLBF && mode != _IONBF)
		return EOF;

	if ( stream->_buf && io_testflag(stream,_IOMYBUF) )
		free((void *)stream->_buf);

	stream->_flags &= ~(_IOMYBUF | _IONBF | _IOLBF);

	if (!buf && (mode != _IONBF))
		if ((buf = (char *) malloc(size)) == NULL) retval = -1;

	stream->_buf = (unsigned char *) buf;

	stream->_count = 0;
	stream->_flags |= mode;
	stream->_ptr = stream->_buf;

	if (!buf) {
		stream->_bufsiz = 1;
	} else {
		if (io_testflag(stream, _IOWRITE)
		    && !io_testflag(stream, _IOLBF))
			stream->_count = size;
		stream->_bufsiz = size;
	}

	return retval;
}
