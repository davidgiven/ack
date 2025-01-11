#include "lib.h"
#define read _read
#include <unistd.h>

PUBLIC ssize_t read(int fd, void* buffer, size_t nbytes)
{
	return (_callm1(FS, READ, fd, nbytes, 0, buffer, NIL_PTR, NIL_PTR));
}
