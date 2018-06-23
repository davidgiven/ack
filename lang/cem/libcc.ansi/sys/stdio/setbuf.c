/*
 * setbuf.c - control buffering of a stream
 */
/* $Id$ */

#include <stdio.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

void setbuf(register FILE* stream, char* buf)
{
	(void)setvbuf(stream, buf, (buf ? _IOFBF : _IONBF), (size_t)BUFSIZ);
}

#endif
