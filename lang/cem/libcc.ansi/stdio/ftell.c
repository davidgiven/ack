/*
 * ftell.c - obtain the value of the file-position indicator of a stream
 */
/* $Header$ */

#include	<stdio.h>
#include	<sys/file.h>
#include	"loc_incl.h"

int lseek(int d, int offset, int whence);

long ftell(FILE * stream)
{
	long result;
	int adjust;

	if ( io_testflag(stream,_IOREAD) )
		adjust = -stream->_count;
	else if (io_testflag(stream,_IOWRITE)
		    && stream->_buf
		    && !io_testflag(stream,_IONBF))
		adjust = stream->_ptr - stream->_buf;
	else
		return -1L;
	
	result = lseek(fileno(stream), 0, L_INCR);

	if ( result == -1 )
		return result;

	result += (long) adjust;
	return result;
}
