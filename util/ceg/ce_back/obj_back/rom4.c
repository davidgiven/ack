#include "mach.h"
#include "back.h"

#undef rom2

#ifdef BYTES_REVERSED
#define rom2(w)		{ *data++ = ((w) >> 8); *data++ = (w);}
#else
#define rom2(w)		{ *data++ = (w); *data++ = ((w)>>8);}
#endif

rom4( l)
FOUR_BYTES l;
{
	if ((data_cnt -= 4) < 0) mem_data();
#ifdef WORDS_REVERSED
	rom2( (int)(l>>16));
	rom2( (int) l);
#else
	rom2( (int) l);
	rom2( (int) (l>>16));
#endif
}


