#include <lib.h>

#define	PTRSIZE	sizeof(char *)
_PROTOTYPE( int _execn, (char * name));

PUBLIC int _execn(name)
char *name;			/* pointer to file to be exec'd */
{
/* Special version used when there are no args and no environment.  This call
 * is principally used by INIT, to avoid having to allocate ARG_MAX.
 */

  PRIVATE char stack[3 * PTRSIZE];

  return(_callm1(MM, EXEC, _len(name), sizeof(stack), 0, name, stack, NIL_PTR));
}
