#include <system.h>
#include "mach.h"
#include "back.h"
 
gen2( w)
TWO_BYTES w;
{
	switch ( cur_seg) {
		case SEGTXT :
				if ((_text_cnt -= 2) < 0) mem_text();
#ifdef BYTES_REVERSED
				*text++ = w>>8;
				*text++ = w;
#else
				*text++ = w;
				*text++ = w>>8;
#endif
			        return;
		case SEGCON  :
		case SEGROM  :
				if ((_data_cnt -= 2) < 0) mem_data();
#ifdef BYTES_REVERSED
				*data++ = w>>8;
				*data++ = w;
#else
				*data++ = w;
				*data++ = w>>8;
#endif
			       return;
		case SEGBSS  : bss( 2);
			       return;
		default      : fprint( STDERR, "gen2() : bad seg number\n");
			       return;
	}
}
