#include <out.h>
#include <system.h>
#include "data.h"
#include "hash.h"

char *realloc();

mem_text()
{
	/* print( "text_area too small %d %d \n", text_area, text); */

	text_area = realloc( text_area, sizeof( char) * 2 * size_text);
	text = text_area + size_text;
	size_text = 2 * size_text;
}


mem_data()
{
	/* print( "data_area too small\n"); */

	data_area = realloc( data_area, sizeof( char) * 2 * size_data);
	data = data_area + size_data;
	size_data = 2 * size_data;
}


mem_symbol_hash()
{
	/* print( "symbol_table out of memory\n"); */

	size_symbol = 2 * size_symbol;
	symbol_table = (struct outname *) realloc( (char *) symbol_table,
				         sizeof( struct outname) * size_symbol);

	/* print( "hash out of memory\n"); */

	Hashitems = (struct Hashitem *) realloc( (char *) Hashitems,
				      sizeof( struct Hashitem)*(size_symbol+1));
}


mem_relo()
{
	/* print( "reloc_table out of memory\n"); */

	reloc_info = (struct outrelo *) realloc( (char *) reloc_info,
				 sizeof( struct outrelo) * 2 * size_reloc);
	relo = reloc_info + size_reloc;
	size_reloc = 2 * size_reloc;
}


mem_string()
{
	int i;

	/* print( "string_area out of memory %d %d \n", string_area, string);*/

	i = string - string_area;
	size_string = 2 * size_string;
	string_area = realloc( string_area, sizeof( char) * size_string);
	string = string_area + i;
}
