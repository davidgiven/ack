#include <system.h>
#include "mach.h"
#include "back.h"
 
gen4( c)
FOUR_BYTES c;
{
	switch ( cur_seg) {
		case SEGTXT :  text4( c);
			       return;
		case SEGCON  : con4( c);
			       return;
		case SEGROM  : rom4( c);
			       return;
		case SEGBSS  : bss( 4);
			       return;
		default      : fprint( STDERR, "gen4() : bad seg number\n");
			       return;
	}
}
