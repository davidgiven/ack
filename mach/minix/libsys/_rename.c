#include <lib.h>
#define rename	_rename

PUBLIC int rename(name, name2)
char *name, *name2;
{
  return(_callm1(FS, RENAME, _len(name), _len(name2), 0,
	 (char *) name, (char *) name2,	/* perhaps callm1 preserves these */
	 NIL_PTR));
}
