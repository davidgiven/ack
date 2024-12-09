#include "header.h"

gen2( w)
TWO_BYTES w;
{
	switch ( cur_seg) {
	  case SEGTXT : fprintf( codefile, WORD_FMT, (long) w);
			break;
	  case SEGCON : fprintf( codefile, WORD_FMT, (long) w);
			break;
	  case SEGROM : fprintf( codefile, WORD_FMT, (long) w);
			break;
	  case SEGBSS : bss( (arith) 2);
			break;
	  default : fprintf( stderr, "gen2 unkown seg %d\n", cur_seg);
	}
}
