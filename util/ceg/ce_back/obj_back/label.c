#include <out.h>
#include "header.h"
#include "back.h"

int  _Label, _Label_waiting; 

save_label( lab)
char *lab;

/* It is now not possible to tell where the label belongs to, so store
 * the string and remember the returned index to store the missing
 * information later on (see dump_label()). Two labels at one address
 * is not allowed.
 */
{
	_Label = find_sym( lab, SYMBOL_DEFINITION);
	_Label_waiting = 1;
}


dump_label()
{
	if ( _Label_waiting) {
		align_word();
	        symbol_table[ _Label].on_valu = cur_value();
	        symbol_table[ _Label].on_type |= ( S_MIN + conv_seg( cur_seg));
		_Label_waiting = 0;
	}
}

