#include <lib.h>
#define rename	_rename
#include <stdio.h>

PUBLIC int rename(name, name2)
_CONST char *name, *name2;
{
  return(_callm1(FS, RENAME, _len(name), _len(name2), 0,
	 (char *) name, (char *) name2,	/* perhaps callm1 preserves these */
	 NIL_PTR));
}
