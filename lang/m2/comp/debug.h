/* A debugging macro
*/

#include "debugcst.h"

#ifdef DEBUG
#define DO_DEBUG(x, y)	((x) && (y))
#else
#define DO_DEBUG(x, y)
#endif
