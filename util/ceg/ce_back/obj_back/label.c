#include <out.h>
#include "header.h"
#include "back.h"

int  Label, Label_waiting; 

save_label( lab)
char *lab;

/* It is now not possible to tell where the label belongs to, so store
 * the string and remember the returned index to store the missing
 * information later on (see dump_label()). Two labels at one address
 * is not allowed.
 */
{
	Label = find_sym( lab, SYMBOL_DEFINITION);
	Label_waiting = 1;
}


dump_label()
{
	if ( Label_waiting) {
		align_word();
	        symbol_table[ Label].on_valu = cur_value();
	        symbol_table[ Label].on_type |= ( S_MIN + conv_seg( cur_seg));
		Label_waiting = 0;
	}
}

