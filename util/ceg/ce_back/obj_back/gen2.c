#include "mach.h"
#include "back.h"
	
gen2( w)
TWO_BYTES w;
{
#ifdef BYTES_REVERSED
	gen1( (char) ( ( unsigned short)w>>8));
	gen1( (char) w);
#else
	gen1( (char) w);
	gen1( (char) ( ( unsigned short)w>>8));
#endif
}

