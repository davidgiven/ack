#include "mach.h"
#include "back.h"

text2( w)
TWO_BYTES w;
{
#ifdef BYTES_REVERSED
	text1( (char) ( ( unsigned short)w>>8));
	text1( (char) w);
#else
	text1( (char) w);
	text1( (char) ( ( unsigned short)w>>8));
#endif
}

