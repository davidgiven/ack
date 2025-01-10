#include "lib.h"
#define chown _chown
#include <unistd.h>

PUBLIC int chown(char* name, int owner, int grp)
{
	return (_callm1(FS, CHOWN, _len(name), owner, grp, name, NIL_PTR, NIL_PTR));
}
