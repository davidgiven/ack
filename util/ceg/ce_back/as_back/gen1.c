#include "header.h"

gen1( w)
arith w;
{
	switch ( cur_seg) {
	  case SEGTXT : fprint( codefile, BYTE_FMT, (arith) w);
			break;
	  case SEGCON : fprint( codefile, BYTE_FMT, (arith) w);
			break;
	  case SEGROM : fprint( codefile, BYTE_FMT, (arith) w);
			break;
	  case SEGBSS : bss( 1);
			break;
	  default : fprint( STDERR, "gen1 unkown seg %d\n", cur_seg);
	}
}
