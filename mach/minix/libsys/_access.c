#include <lib.h>
#define access	_access

int access(name, mode)
char *name;
int mode;
{
  return(_callm3(FS, ACCESS, mode, name));
}
