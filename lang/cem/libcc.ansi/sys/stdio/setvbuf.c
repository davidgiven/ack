/*
 * setbuf.c - control buffering of a stream
 */
/* $Id$ */

#include <stdio.h>
#include <stdlib.h>

#if ACKCONF_WANT_STDIO && ACKCONF_WANT_EMULATED_FILE

int setvbuf(register FILE* stream, char* buf, int mode, size_t size)
{
	int retval = 0;

	__register_stdio_cleanup();
	if (mode != _IOFBF && mode != _IOLBF && mode != _IONBF)
		return EOF;

	if (stream->_buf && io_testflag(stream, _IOMYBUF))
		free((void*)stream->_buf);

	stream->_flags &= ~(_IOMYBUF | _IONBF | _IOLBF);

	if (buf && size <= 0)
		retval = EOF;
	if (!buf && (mode != _IONBF))
	{
		if (size <= 0 || (buf = (char*)malloc(size)) == NULL)
		{
			retval = EOF;
		}
		else
		{
			stream->_flags |= _IOMYBUF;
		}
	}

	stream->_buf = (unsigned char*)buf;

	stream->_count = 0;
	stream->_flags |= mode;
	stream->_ptr = stream->_buf;

	if (!buf)
	{
		stream->_bufsiz = 1;
	}
	else
	{
		stream->_bufsiz = size;
	}

	return retval;
}

#endif
