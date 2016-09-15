/* A debugging macro
*/

#ifdef DEBUG
#define DO_DEBUG(x, y)	((x) && (y))
#else
#define DO_DEBUG(x, y)
#endif
