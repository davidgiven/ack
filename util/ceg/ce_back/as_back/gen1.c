#include "header.h"

gen1( w)
ONE_BYTE w;
{
	switch ( cur_seg) {
	  case SEGTXT : fprint( codefile, BYTE_FMT, (long) w);
			break;
	  case SEGCON : fprint( codefile, BYTE_FMT, (long) w);
			break;
	  case SEGROM : fprint( codefile, BYTE_FMT, (long) w);
			break;
	  case SEGBSS : bss( (arith) 1);
			break;
	  default : fprint( STDERR, "gen1 unkown seg %d\n", cur_seg);
	}
}
