#include <system.h>
#include "mach.h"
#include "back.h"
 
gen2( c)
TWO_BYTES c;
{
	switch ( cur_seg) {
		case SEGTXT :  text2( c);
			       return;
		case SEGCON  : con2( c);
			       return;
		case SEGROM  : rom2( c);
			       return;
		case SEGBSS  : bss( 2);
			       return;
		default      : fprint( STDERR, "gen2() : bad seg number\n");
			       return;
	}
}
