#include <system.h>
#include <em.h>
#include "mach.h"
#include "back.h"
#ifdef BYTES_REVERSED
#define text2(w)	{ *text++ = ((w) >> 8); *text++ = (w);}
#define con2(w)		{ *data++ = ((w) >> 8); *data++ = (w);}
#else
#define text2(w)	{ *text++ = (w); *text++ = ((w)>>8);}
#define con2(w)		{ *data++ = (w); *data++ = ((w)>>8);}
#endif
 
gen4( l)
FOUR_BYTES l;
{
	switch ( cur_seg) {
		case SEGTXT :
			if ((_text_cnt -= 4) < 0) mem_text();
#ifdef WORDS_REVERSED
			text2( (int) (l>>16));
			text2( (int) l);
#else
			text2( (int) l);
			text2( (int) (l>>16));
#endif
			return;
		case SEGCON  :
		case SEGROM  :
			if ((_data_cnt -= 4) < 0) mem_data();
#ifdef WORDS_REVERSED
			con2( (int)(l>>16));
			con2( (int) l);
#else
			con2( (int) l);
			con2( (int) (l>>16));
#endif
			return;
		case SEGBSS  : bss( (arith) 4);
			       return;
		default      : fprint( STDERR, "gen4() : bad seg number\n");
			       return;
	}
}
