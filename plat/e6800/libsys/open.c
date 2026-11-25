#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int open(const char* path, int access, ...)
{
	static int fd = 0;

	if (access & _IOWRITE)
	  return 1;

	return 0;
}


