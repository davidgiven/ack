#include <system.h>
#include "mach.h"
#include "back.h"
 
gen1( c)
ONE_BYTE c;
{
	switch ( cur_seg) {
		case SEGTXT :  text1( c);
			       return;
		case SEGCON  : con1( c);
			       return;
		case SEGROM  : rom1( c);
			       return;
		case SEGBSS  : bss( 1);
			       return;
		default      : fprint( STDERR, "gen1() : bad seg number\n");
			       return;
	}
}

