#include <pc_file.h>

extern                _cls();   /* pc runtime routine to close a file */

/* as the following routine is called from a pascal subroutine */
/* and the pascal compiler has been fixed to alwayd supply static links */
/* to non-global externals the parameter 'statlink' is a dummy to fill */
/* the space occupied by the static link. The parameter is first instead */
/* of last because of the C method of passing its parameters backwards */

ACLS(statlink,f) int *statlink; struct file *f; {

	_cls(f);
}

