#include "mach.h"

gen4( l)
FOUR_BYTES l;
{
#ifdef WORDS_REVERSED
	gen2( (short) ((unsigned long)l>>16));
	gen2( (short) l);
#else
	gen2( (short) l);
	gen2( (short) ((unsigned long)l>>16));
#endif
}

