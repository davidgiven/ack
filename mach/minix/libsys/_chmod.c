#include <lib.h>
#define chmod	_chmod

PUBLIC int chmod(name, mode)
char *name;
mode_t mode;
{
  return(_callm3(FS, CHMOD, mode, name));
}
