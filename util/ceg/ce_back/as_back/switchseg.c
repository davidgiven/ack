#include "header.h"

switchseg( seg)
int seg;
{	
	if ( seg == cur_seg)
		return;

	cur_seg = seg;

	switch ( cur_seg) {
	   case SEGTXT : fprintf( codefile, SEGTXT_FMT);
			 break;
	   case SEGCON : fprintf( codefile, SEGDAT_FMT);
			 break;
	   case SEGROM : fprintf( codefile, SEGDAT_FMT);
			 break;
	   case SEGBSS : fprintf( codefile, SEGBSS_FMT);
			 break;
	}

	if ( seg != SEGTXT)
		align_word();
}

