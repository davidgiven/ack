#include "f2c.h"

extern integer s_cmp();

shortint hl_lt(a,b,la,lb)
char *a, *b;
long int la, lb;
{
return(s_cmp(a,b,la,lb) < 0);
}
