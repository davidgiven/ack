#include <out.h>
#include "back.h"
#include "hash.h"
#include <alloc.h>

char *calloc();

init_back()

/* Allocate space for the tables and set the default values.
 */
{
	text_area = Malloc( MAXTEXT);
	data_area = Malloc( MAXDATA);
	reloc_info = (struct outrelo *)Malloc( MAXRELO* sizeof(struct outrelo));
	symbol_table = (struct outname *)Malloc( MAXNAME* sizeof(struct outname));
	Hashitems = (struct Hashitem *)Malloc( (MAXNAME + 1)*
						sizeof( struct Hashitem));
 	string_area = Malloc( MAXSTRING);

	text = text_area;
	data = data_area;
	string = string_area;
        relo = reloc_info;

	size_text = MAXTEXT;
	text_cnt = MAXTEXT;
	size_data = MAXDATA;
	data_cnt = MAXDATA;
	size_reloc = MAXRELO;
	size_symbol = MAXNAME;
	size_string = MAXSTRING;
}
