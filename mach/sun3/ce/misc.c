#include <system.h>
#include "mach.h"
#include "back.h"

/* The following functions are called from reloc1(), reloc2(), reloc4(),
 * dump_label().
 */

align_word()

/* Do word allignment.
 */
{
	switch ( cur_seg) {	
		case SEGTXT : return;
		case SEGCON : if ( (data - data_area) % 2 != 0) 
					con1( '\0');
			      return;
		case SEGROM : if ( (data - data_area) % 2 != 0) 
					rom1( '\0');
			      return;
		case SEGBSS : if ( nbss % 2 != 0) 
					nbss++;
			      return;
		default     : fprint( STDERR, "align_word() : unknown seg\n");
			      return;
	}
}


long cur_value()

/* Return the index of the first free entry.
 */
{
	switch( cur_seg) {
		case SEGTXT: return text - text_area;
		case SEGCON: return data - data_area;
		case SEGROM: return data - data_area;
		case SEGBSS: return nbss;
		default    : fprint( STDERR, "cur_value() : unknown seg\n");
			     return -1L;
	}
}

