#include "mach.h"

rom4( l)
FOUR_BYTES l;
{
#ifdef WORDS_REVERSED
	rom2( (short) ((unsigned long)l>>16));
	rom2( (short) l);
#else
	rom2( (short) l);
	rom2( (short) ((unsigned long)l>>16));
#endif
}


