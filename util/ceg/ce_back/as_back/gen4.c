#include "header.h"

gen4( w)
FOUR_BYTES w;
{
	switch ( cur_seg) {
	  case SEGTXT : fprintf( codefile, LONG_FMT, (long) w);
			break;
	  case SEGCON : fprintf( codefile, LONG_FMT, (long) w);
			break;
	  case SEGROM : fprintf( codefile, LONG_FMT, (long) w);
			break;
	  case SEGBSS : bss( (arith) 4);
			break;
	  default : fprintf( stderr, "gen4 unkown seg %d\n", cur_seg);
	}
}
