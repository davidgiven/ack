#include "header.h"

switchseg( seg)
int seg;
{	
	if ( seg == cur_seg)
		return;

	cur_seg = seg;

	switch ( cur_seg) {
	   case SEGTXT : fprint( codefile, SEGTXT_FMT);
			 break;
	   case SEGCON : fprint( codefile, SEGDAT_FMT);
			 break;
	   case SEGROM : fprint( codefile, SEGDAT_FMT);
			 break;
	   case SEGBSS : fprint( codefile, SEGBSS_FMT);
			 break;
	}

	if ( seg != SEGTXT)
		align_word();
}

