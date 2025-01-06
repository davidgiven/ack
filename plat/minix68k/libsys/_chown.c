#include <lib.h>
#define chown	_chown
#include <unistd.h>

PUBLIC int chown(name, owner, grp)
char *name;
int owner, grp;
{
  return(_callm1(FS, CHOWN, _len(name), owner, grp, name, NIL_PTR, NIL_PTR));
}
