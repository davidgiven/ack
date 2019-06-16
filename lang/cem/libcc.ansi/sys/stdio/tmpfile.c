/*
 * tmpfile.c - create and open a temporary file
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#if ACKCONF_WANT_STDIO

FILE* tmpfile(void)
{
	static char name_buffer[L_tmpnam] = ACK_TMP_PREFIX;
	static char* name = NULL;
	FILE* file;

	if (!name)
	{
		name = name_buffer + strlen(name_buffer);
		name = _i_compute(getpid(), 10, name, 5);
		*name = '\0';
	}

	file = fopen(name_buffer, "wb+");
	if (!file)
		return (FILE*)NULL;
	(void)remove(name_buffer);
	return file;
}

#endif
