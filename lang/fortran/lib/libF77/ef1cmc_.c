/* EFL support routine to compare two character strings */

#include "f2c.h"

extern integer s_cmp();

integer ef1cmc_(a, la, b, lb)
integer *a, *b;
integer *la, *lb;
{
return( s_cmp( (char *)a, (char *)b, *la, *lb) );
}
