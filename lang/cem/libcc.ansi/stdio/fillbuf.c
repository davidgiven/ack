/*
 * fillbuf.c - fill a buffer
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "loc_incl.h"

int
__fillbuf(register FILE *stream)
{
	static unsigned char ch[FOPEN_MAX];
	register int i;

	stream->_count = 0;
	if (fileno(stream) < 0) return EOF;
	if (io_testflag(stream, _IOEOF)) return EOF;
	if (!io_testflag(stream, _IOREAD)) {
		stream->_flags |= _IOERR;
		return EOF;
	}
	if (io_testflag(stream, _IOWRITING)) {
		stream->_flags |= _IOERR;
		return EOF;
	}

	if (!io_testflag(stream, _IOREADING))
		stream->_flags |= _IOREADING;
	
	if (!io_testflag(stream, _IONBF) && !stream->_buf) {
		stream->_buf = (unsigned char *) malloc(BUFSIZ);
		if (!stream->_buf) {
			stream->_flags |= _IONBF;
		}
		else {
			stream->_flags |= _IOMYBUF;
			stream->_bufsiz = BUFSIZ;
		}
	}

	/* flush line-buffered output when filling an input buffer */
	for (i = 0; i < FOPEN_MAX; i++) {
		if (__iotab[i] && io_testflag(__iotab[i], _IOLBF))
			if (io_testflag(__iotab[i], _IOWRITING))
				(void) fflush(__iotab[i]);
	}

	if (!stream->_buf) {
		stream->_buf = &ch[fileno(stream)];
		stream->_bufsiz = 1;
	}
	stream->_ptr = stream->_buf;
	stream->_count = read(stream->_fd, (char *)stream->_buf, stream->_bufsiz);

	if (stream->_count <= 0){
		if (stream->_count == 0) {
			stream->_flags |= _IOEOF;
		}
		else 
			stream->_flags |= _IOERR;

		return EOF;
	}
	stream->_count--;

	return *stream->_ptr++;
}
