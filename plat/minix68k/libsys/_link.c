#include "lib.h"
#define link _link
#include <unistd.h>

PUBLIC int link(_CONST char* name, _CONST char* name2)
{
	return (_callm1(
	    FS, LINK, _len(name), _len(name2), 0, (char*)name,
	    (char*)name2, /* perhaps callm1 preserves these */
	    NIL_PTR));
}
