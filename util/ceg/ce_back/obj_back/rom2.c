#include "mach.h"
#include "back.h"

rom2( w)
TWO_BYTES w;
{
#ifdef BYTES_REVERSED
	rom1( (char) ( ( unsigned short)w>>8));
	rom1( (char) w);
#else
	rom1( (char) w);
	rom1( (char) ( ( unsigned short)w>>8));
#endif
}


