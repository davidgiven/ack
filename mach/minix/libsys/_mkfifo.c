#include <lib.h>
#include <sys/types.h>
#define mkfifo	_mkfifo
#include <sys/stat.h>

PUBLIC int mkfifo(name, mode)
char *name;
int mode;
{
  mode = (mode & 0777) | S_IFIFO;
  return(_callm1(FS, MKNOD, _len(name), (int)mode, 0,
	 (char *)name, NIL_PTR, NIL_PTR));
}
