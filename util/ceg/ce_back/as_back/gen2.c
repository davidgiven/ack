#include "header.h"

gen2( w)
TWO_BYTES w;
{
	switch ( cur_seg) {
	  case SEGTXT : fprint( codefile, WORD_FMT, (long) w);
			break;
	  case SEGCON : fprint( codefile, WORD_FMT, (long) w);
			break;
	  case SEGROM : fprint( codefile, WORD_FMT, (long) w);
			break;
	  case SEGBSS : bss( (arith) 2);
			break;
	  default : fprint( STDERR, "gen2 unkown seg %d\n", cur_seg);
	}
}
