/*
 * fflush.c - flush stream(s)
 */
/* $Header$ */

#include	<stdio.h>
#include	"loc_incl.h"

int write(int d, const char *buf, int nbytes);

int
fflush(FILE *stream)
{
	int count, c1, i, retval = 0;

	if (!stream) {
	    for(i= 0; i < FOPEN_MAX; i++)
		if (__iotab[i] && fflush(__iotab[i]))
			retval = EOF;
	    return retval;
	}

	if (!stream->_buf
	    || io_testflag(stream, _IONBF)
	    || !io_testflag(stream, _IOWRITE)
	    || !io_testflag(stream, _IOWRITING)) 
		return 0;

	if (io_testflag(stream, _IOREAD))		/* "a" or "+" mode */
		stream->_flags &= ~_IOWRITING;

	/*
	if (io_testflag(stream, _IOLBF))
		count = -stream->_count;
	else	count = stream->_bufsiz - stream->_count;
	*/
	count = stream->_ptr - stream->_buf;
	stream->_ptr = stream->_buf;

	if ( count <= 0 )
		return 0;

	c1 = write(stream->_fd, (char *)stream->_buf, count);

	stream->_count = 0;

	/*
	if(stream != stderr)
		fprintf(stderr,"written %d bytes :\"%.*s\"\n"
				, c1, c1, stream->_buf);
	*/

	if ( count == c1 )
		return 0;

	stream->_flags |= _IOERR;
	return EOF; 
}
