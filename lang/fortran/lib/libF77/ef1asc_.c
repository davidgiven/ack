/* EFL support routine to copy string b to string a */

#include "f2c.h"

extern VOID s_copy();

#define M	( (long) (sizeof(long) - 1) )
#define EVEN(x)	( ( (x)+ M) & (~M) )

VOID ef1asc_(a, la, b, lb)
int *a, *b;
long int *la, *lb;
{
s_copy( (char *)a, (char *)b, EVEN(*la), *lb );
}
