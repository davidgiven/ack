/* When the signal call is made, a pointer to the function to be called
 * is stored in this table.  When the interrupt comes in from the kernel,
 * the function is called within the user address space using _vectab[].
 */

#include <lib.h>
#include <signal.h>

/* array of functions to catch signals */
_PROTOTYPE( void (*_vectab[_NSIG]), (int));
