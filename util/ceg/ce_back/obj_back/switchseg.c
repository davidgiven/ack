#include "data.h"

switchseg( seg)
int seg;

/* De EM definitie eist dat pseudoinstructies gealigned worden op
 * woordgrens.
 */
{	
	if ( seg == cur_seg)
		return;
	cur_seg = seg;
	align_word();
}
