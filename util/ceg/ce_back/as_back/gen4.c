#include "header.h"

gen4( w)
arith w;
{
	switch ( cur_seg) {
	  case SEGTXT : fprint( codefile, LONG_FMT, (arith) w);
			break;
	  case SEGCON : fprint( codefile, LONG_FMT, (arith) w);
			break;
	  case SEGROM : fprint( codefile, LONG_FMT, (arith) w);
			break;
	  case SEGBSS : bss( (arith) 4);
			break;
	  default : fprint( STDERR, "gen4 unkown seg %d\n", cur_seg);
	}
}
