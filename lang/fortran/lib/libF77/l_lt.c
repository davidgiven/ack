#include "f2c.h"

integer l_lt(a,b,la,lb)
char *a, *b;
long la, lb;
{
return(s_cmp(a,b,la,lb) < 0);
}
