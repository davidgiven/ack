#include <out.h>
#include <system.h>
#include "back.h"
#include "hash.h"
#include <alloc.h>


/* The routines allocate more space for the segments and update the
 * global variables. Each time the space asked for is multiplied with 2.
 */

mem_text()
{
	/* printf( "text_area too small %d %d \n", text_area, text); */
	int diff = text - text_area;

	text_area = realloc( text_area, sizeof( char) * 2 * size_text);
	text = text_area + diff;
	text_cnt += size_text;
	size_text = 2 * size_text;
}


mem_data()
{
	/* printf( "data_area too small\n"); */
	int diff = data - data_area;

	data_area = realloc( data_area, sizeof( char) * 2 * size_data);
	data = data_area + diff;
	data_cnt += size_data;
	size_data = 2 * size_data;
}


mem_symbol_hash()
{
	/* printf( "symbol_table out of memory\n"); */

	size_symbol = 2 * size_symbol;
	symbol_table = (struct outname *) realloc( (char *) symbol_table,
				         sizeof( struct outname) * size_symbol);

	/* printf( "hash out of memory\n"); */

	Hashitems = (struct Hashitem *) realloc( (char *) Hashitems,
				      sizeof( struct Hashitem)*(size_symbol+1));
}


mem_relo()
{
	/* printf( "reloc_table out of memory\n"); */
	int diff = relo - reloc_info;

	reloc_info = (struct outrelo *) realloc( (char *) reloc_info,
				 sizeof( struct outrelo) * 2 * size_reloc);
	relo = reloc_info + diff;
	size_reloc = 2 * size_reloc;
}


mem_string()
{
	int diff = string - string_area;

	/* printf( "string_area out of memory %d %d \n", string_area, string);*/

	size_string = 2 * size_string;
	string_area = realloc( string_area, sizeof( char) * size_string);
	string = string_area + diff;
}
