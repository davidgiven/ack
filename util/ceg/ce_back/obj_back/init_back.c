#include <out.h>
#include "back.h"
#include "hash.h"

char *calloc();

init_back()

/* Allocate space for the tables and set the default values.
 */
{
	text_area = calloc( MAXTEXT, sizeof( char));
	data_area = calloc( MAXDATA, sizeof( char));
	reloc_info = (struct outrelo *)calloc( MAXRELO, SZ_RELO);
	symbol_table = (struct outname *)calloc( MAXNAME, SZ_NAME);
	Hashitems = (struct Hashitem *)calloc( MAXNAME + 1, 
						sizeof( struct Hashitem));
 	string_area = calloc( MAXSTRING, sizeof( char));

	text = text_area;
	data = data_area;
	string = string_area;
        relo = reloc_info;

	size_text = MAXTEXT;
	size_data = MAXDATA;
	size_reloc = MAXRELO;
	size_symbol = MAXNAME;
	size_string = MAXSTRING;
}
