#include "lib.h"
#define write _write
#include <unistd.h>

PUBLIC ssize_t write(int fd, const void* buffer, size_t nbytes)
{
	return _callm1(FS, WRITE, fd, nbytes, 0, (char*)buffer, NULL, NULL);
}
