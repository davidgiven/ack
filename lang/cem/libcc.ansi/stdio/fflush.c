/*
 * fflush.c - flush stream(s)
 */
/* $Header$ */

#include	<stdio.h>
#include	"loc_incl.h"

int write(int d, static char *buf, int nbytes);

int
fflush(FILE *stream)
{
	int count, c1, i, retval = 0;

	if (!stream) {
	    for(i= 0; i < FOPEN_MAX; i++)
		if (_iotable[i] && fflush(_iotable[i]))
			retval = EOF;
	    return retval;
	}

	if (!stream->_buf ||
	    io_testflag(stream,_IONBF) ||
	    !io_testflag(stream,_IOWRITE) ) 
		return(0);

	if (io_testflag(stream, _IOLBF))
		count = -stream->_count;
	else	count = stream->_bufsiz - stream->_count;

	if ( count <= 0 )
		return(0);

	c1 = write(stream->_fd, (char *)stream->_buf, count);

	if ( count == c1 ) {
		if (io_testflag(stream, _IOLBF))
			stream->_count = 0;
		else	stream->_count = stream->_bufsiz;
		stream->_ptr   = stream->_buf;
		return(count);
	}

	stream->_flags |= _IOERR;
	return(EOF); 
}
