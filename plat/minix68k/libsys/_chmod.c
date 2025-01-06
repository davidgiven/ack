#include <lib.h>
#define chmod	_chmod
#include <sys/stat.h>

PUBLIC int chmod(name, mode)
_CONST char *name;
mode_t mode;
{
  return(_callm3(FS, CHMOD, mode, name));
}
