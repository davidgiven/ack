/*
 * fillbuf.c - fill a buffer
 */
/* $Header$ */

#include	<stdio.h>
#include	<stdlib.h>
#include	"loc_incl.h"

int read(int d, char *buf, int nbytes);

int
_fillbuf(register FILE *stream)
{
	static unsigned char ch[FOPEN_MAX];

	stream->_count = 0;
	if (fileno(stream) < 0) return EOF;
	if (io_testflag(stream, (_IOEOF | _IOERR )))
		return (EOF); 

	if (!io_testflag(stream, _IOREAD) ) 
		return (EOF);

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

		return (EOF);
	}
	stream->_count--;

	return *stream->_ptr++;
}
