/* $Header$ */

#include <sys/types.h>
#include <sys/stat.h>

long
sys_modtime(path)
	char *path;
{
	struct stat st_buf;

	if (stat(path, &st_buf) != 0)
		return -1L;
	return (long) st_buf.st_mtime;
}
