/*
 * fileno .c - map a stream to a file descriptor
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int(fileno)(FILE* stream)
{
	return stream->_fd;
}

#endif
