#include "f2c.h"

integer iargc_()
{
extern int xargc;
return ( xargc - 1 );
}
