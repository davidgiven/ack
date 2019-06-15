/*
 * fclose.c - flush a stream and close the file
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO && ACKCONF_WANT_EMULATED_FILE

int fclose(FILE* fp)
{
	register int i, retval = 0;

	for (i = 0; i < FOPEN_MAX; i++)
		if (fp == __iotab[i])
		{
			__iotab[i] = 0;
			break;
		}
	if (i >= FOPEN_MAX)
		return EOF;
	if (fflush(fp))
		retval = EOF;
	if (close(fileno(fp)))
		retval = EOF;
	if (io_testflag(fp, _IOMYBUF) && fp->_buf)
		free((void*)fp->_buf);
	if (fp != stdin && fp != stdout && fp != stderr)
		free((void*)fp);
	return retval;
}

#endif
