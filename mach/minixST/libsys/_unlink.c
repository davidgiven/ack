#include <lib.h>
#define unlink	_unlink
#include <unistd.h>

PUBLIC int unlink(name)
_CONST char *name;
{
  return(_callm3(FS, UNLINK, 0, name));
}
