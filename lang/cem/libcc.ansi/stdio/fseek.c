/*
 * fseek.c - perform an fseek
 */
/* $Header$ */

#include	<stdio.h>
#include	<sys/file.h>
#include	"loc_incl.h"

int lseek(int d, int offset, int whence);

int
fseek(FILE *stream, long int offset, int whence)
{
	int count;
	long pos;

#if	(SEEK_CUR != L_INCR) || (SEEK_SET != L_SET) || (SEEK_END != L_XTND)
	int swhence;				/* the real whence for lseek */

	switch(whence) {			/* map to UNIX values */
	case SEEK_CUR:	swhence = L_INCR;	break;
	case SEEK_SET:	swhence = L_SET;	break;
	case SEEK_END:	swhence = L_XTND;	break;
	default:	swhence = whence;	break;
	}
#else
#define	swhence		whence
#endif

	stream->_flags &= ~(_IOEOF | _IOERR);
	/* Clear both the end of file and error flags */

	if ( io_testflag(stream,_IOREAD) ) {
		if ( whence < 2 && stream->_buf
		    && !io_testflag(stream,_IONBF) ) {
			count = stream->_count;
			pos = offset;

			if ( whence == SEEK_SET )
				pos +=
				    count - lseek(fileno(stream), 0L, swhence);
			else
				offset -= count;

			if ( count > 0 && pos <= count 
			     && pos >= stream->_buf - stream->_ptr ) {
		        	stream->_ptr += (int) pos;
				stream->_count -= (int) pos;
				return(0);
			}
		}
		pos = lseek(fileno(stream), offset, swhence);
		stream->_count = 0;
	} else if ( io_testflag(stream,_IOWRITE) ) {
		fflush(stream);
		pos = lseek(fileno(stream), offset, swhence);
	}
	return((pos == -1) ? -1 : 0 );
}
