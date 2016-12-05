#include <fcntl.h>

int creat(const char *path, mode_t mode)
{
	return open(path, O_CREAT | O_TRUNC | O_WRONLY, mode);
}
