/*
 * fseek.c - perform an fseek
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "loc_incl.h"

int fseek(FILE* stream, long int offset, int whence)
{
	int adjust = 0;
	long pos;

	stream->_flags &= ~(_IOEOF | _IOERR);
	/* Clear both the end of file and error flags */

	if (io_testflag(stream, _IOREADING))
	{
		if (whence == SEEK_CUR
		    && stream->_buf
		    && !io_testflag(stream, _IONBF))
			adjust = stream->_count;
		stream->_count = 0;
	}
	else if (io_testflag(stream, _IOWRITING))
	{
		fflush(stream);
	}
	else /* neither reading nor writing. The buffer must be empty */
		/* EMPTY */;

	pos = lseek(fileno(stream), offset - adjust, whence);
	if (io_testflag(stream, _IOREAD) && io_testflag(stream, _IOWRITE))
		stream->_flags &= ~(_IOREADING | _IOWRITING);

	stream->_ptr = stream->_buf;
	return ((pos == -1) ? -1 : 0);
}
