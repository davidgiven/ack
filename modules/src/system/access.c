/* $Header$ */

#include <system.h>

int
sys_access(path, mode)
	char *path;
	int mode;
{
	return access(path, mode) == 0;
}
