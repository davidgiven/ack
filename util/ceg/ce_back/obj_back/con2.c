#include "mach.h"
#include "back.h"

con2( w)
TWO_BYTES w;
{
#ifdef BYTES_REVERSED
	con1( (char) ( ( unsigned short)w>>8));
	con1( (char) w);
#else
	con1( (char) w);
	con1( (char) ( ( unsigned short)w>>8));
#endif
}

