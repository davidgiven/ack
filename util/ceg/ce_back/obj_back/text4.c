#include "mach.h"
#include "back.h"

#ifdef BYTES_REVERSED
#define text2(w)	{ *text++ = ((w) >> 8); *text++ = (w);}
#else
#define text2(w)	{ *text++ = (w); *text++ = ((w)>>8);}
#endif

text4( l)
FOUR_BYTES l;
{
	if ((_text_cnt -= 4) < 0) mem_text();
#ifdef WORDS_REVERSED
	text2( (int) (l>>16));
	text2( (int) l);
#else
	text2( (int) l);
	text2( (int) (l>>16));
#endif
}
