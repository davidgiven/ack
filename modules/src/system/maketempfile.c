/*  Copyright (c) 2022. See the file License in
 *  the root directory for more information.
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include "system.h"

#if defined WIN32
static unsigned temper(unsigned x)
{
	x ^= x>>11;
	x ^= x<<7 & 0x9D2C5680;
	x ^= x<<15 & 0xEFC60000;
	x ^= x>>18;
	return x;
}

static int rand_r(unsigned *seed)
{
	return temper(*seed = *seed * 1103515245 + 12345)/2;
}
#endif

char* sys_maketempfile(const char* prefix, const char* suffix)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	unsigned int seed = (unsigned int)(uintptr_t)prefix
		^ (unsigned int)ts.tv_sec ^ (unsigned int)ts.tv_nsec;
	const char* tempdir = sys_gettmpdir();

	for (;;)
	{
		unsigned int hash = rand_r(&seed);
		char* filename = aprintf("%s/ack-%s-%x%s",
			tempdir, prefix, hash,
			suffix);
		int fd = open(filename, O_CREAT|O_EXCL|O_RDWR, 0600);
		if (fd != -1)
		{
			close(fd);
			return filename;
		}
		if (errno != EEXIST)
			perror("could not create temporary file");
		free(filename);
	}
}
