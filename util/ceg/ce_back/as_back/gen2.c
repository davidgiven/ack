#include "header.h"

gen2( w)
arith w;
{
	switch ( cur_seg) {
	  case SEGTXT : fprint( codefile, WORD_FMT, (arith) w);
			break;
	  case SEGCON : fprint( codefile, WORD_FMT, (arith) w);
			break;
	  case SEGROM : fprint( codefile, WORD_FMT, (arith) w);
			break;
	  case SEGBSS : bss( (arith) 2);
			break;
	  default : fprint( STDERR, "gen2 unkown seg %d\n", cur_seg);
	}
}
