#include <lib.h>
#define unlink	_unlink

PUBLIC int unlink(name)
char *name;
{
  return(_callm3(FS, UNLINK, 0, name));
}
