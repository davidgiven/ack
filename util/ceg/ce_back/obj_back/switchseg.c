#include "data.h"

switchseg( seg)
int seg;

 /* The EM definition demands that pseudo instructions are aligned
  * at word boundaries.
  */
{	
	if ( seg == cur_seg)
		return;
	cur_seg = seg;
	align_word();
}
