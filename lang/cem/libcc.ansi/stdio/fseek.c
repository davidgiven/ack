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
	int count, adjust = 0;
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

	if (io_testflag(stream, _IOREADING)) {
		if (whence == SEEK_CUR
		    && stream->_buf
		    && !io_testflag(stream,_IONBF))
			adjust = stream->_count;
		pos = lseek(fileno(stream), offset - adjust, swhence);
		stream->_count = 0;
	}
	else if (io_testflag(stream,_IOWRITING)) {
		fflush(stream);
		pos = lseek(fileno(stream), offset, swhence);
	}
	else	/* neither reading nor writing. The buffer must be empty */
		pos = lseek(fileno(stream), offset, swhence);

	if (io_testflag(stream, _IOREAD) && io_testflag(stream, _IOWRITE))
		stream->_flags &= ~(_IOREADING | _IOWRITING);

	stream->_ptr = stream->_buf;
	return ((pos == -1) ? -1 : 0);
}
