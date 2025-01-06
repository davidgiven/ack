#include <lib.h>
#define mknod	_mknod
#include <unistd.h>

PUBLIC int mknod(name, mode, addr)
_CONST char *name;
int mode, addr;
{
  return(_callm1(FS, MKNOD, _len(name), mode, addr,
					(char *) name, (char *) 0, NIL_PTR));
}
