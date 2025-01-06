#include <lib.h>
#define chdir	_chdir
#include <unistd.h>

PUBLIC int chdir(name)
char *name;
{
  return(_callm3(FS, CHDIR, 0, name));
}
