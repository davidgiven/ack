#include <lib.h>
#define mknod4	_mknod4

PUBLIC int mknod4(name, mode, addr, size)
char *name;
int mode, addr;
unsigned int size;
{
  return(_callm1(FS, MKNOD, _len(name), mode, addr, 
				       (char *) name, (char *) size, NIL_PTR));
}
