#include "mach.h"

text4( l)
FOUR_BYTES l;
{
#ifdef WORDS_REVERSED
	text2( (short) ((unsigned long)l>>16));
	text2( (short) l);
#else
	text2( (short) l);
	text2( (short) ((unsigned long)l>>16));
#endif
}
