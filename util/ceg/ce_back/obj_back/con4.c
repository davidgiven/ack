#include "mach.h"

con4( l)
FOUR_BYTES l;
{
#ifdef WORDS_REVERSED
	con2( (short) ((unsigned long)l>>16));
	con2( (short) l);
#else
	con2( (short) l);
	con2( (short) ((unsigned long)l>>16));
#endif
}


