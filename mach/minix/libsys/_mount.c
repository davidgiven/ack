#include <lib.h>
#define mount	_mount

PUBLIC int mount(special, name, rwflag)
char *name, *special;
int rwflag;
{
  return(_callm1(FS, MOUNT, _len(special), _len(name), rwflag, special, name, NIL_PTR));
}
