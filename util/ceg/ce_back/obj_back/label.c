#include <out.h>
#include "header.h"
#include "back.h"

int  Label, label_waiting; 

save_label( lab)
char *lab;
{
	Label = find_sym( lab, SYMBOL_DEFINITION);
	label_waiting = 1;
}


dump_label()
{
	if ( label_waiting) {
		align_word();
	        symbol_table[ Label].on_valu = cur_value();
	        symbol_table[ Label].on_type |= ( S_MIN + conv_seg( cur_seg));
		label_waiting = 0;
	}
}

