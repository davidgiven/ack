#include "header.h"

gen1( w)
ONE_BYTE w;
{
	switch ( cur_seg) {
	  case SEGTXT : fprintf( codefile, BYTE_FMT, (long) w);
			break;
	  case SEGCON : fprintf( codefile, BYTE_FMT, (long) w);
			break;
	  case SEGROM : fprintf( codefile, BYTE_FMT, (long) w);
			break;
	  case SEGBSS : bss( (arith) 1);
			break;
	  default : fprintf( stderr, "gen1 unkown seg %d\n", cur_seg);
	}
}
