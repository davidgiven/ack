#include <lib.h>
#define stat	_stat
#include <sys/stat.h>

PUBLIC int stat(name, buffer)
char *name;
struct stat *buffer;
{
  return(_callm1(FS, STAT, _len(name), 0, 0,
  		(char *)name, (char *)buffer, NIL_PTR));
}
