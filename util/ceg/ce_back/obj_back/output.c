#include <system.h>
#include <out.h>
#include "data.h"

output()
/* Notice : entries in the symbol_table are converted.
 */

{
	struct outhead header;
	struct outsect sect;
        long 	ntext = text - text_area, 
		ndata = data - data_area,
		nchar = string - string_area;
	int 	nrelo = relo - reloc_info;

	header.oh_magic = O_MAGIC;
	header.oh_flags = HF_LINK;
	header.oh_nsect = 4;
	header.oh_nrelo = nrelo;
	header.oh_nname = nname;
	header.oh_nemit = ntext + ndata;
	header.oh_nchar = nchar;

	wr_ohead( &header);

	sect.os_base = 20;
	sect.os_size = ntext;
	sect.os_foff = OFF_EMIT( header);
	sect.os_flen = ntext;
	sect.os_lign = 1;

	wr_sect( &sect, 1);

	sect.os_base = 20 + ntext;
	sect.os_size = 0;
	sect.os_foff = OFF_EMIT( header) + ntext;
	sect.os_flen = 0;
	sect.os_lign = 1;

	wr_sect( &sect, 1);

	sect.os_base = 20 + ntext;
	sect.os_size = ndata;
	sect.os_foff = OFF_EMIT( header) + ntext;
	sect.os_flen = ndata;
	sect.os_lign = 1;

	wr_sect( &sect, 1);

	sect.os_base = 20 + ntext + ndata;
	sect.os_size = nbss;
	sect.os_foff = OFF_EMIT( header) + ntext + ndata;
	sect.os_flen = 0;
	sect.os_lign = 1;

	wr_sect( &sect, 1);

	wr_outsect( 0);
	wr_emit( text_area, ntext);
	wr_outsect( 2);
	wr_emit( data_area, ndata);

	wr_relo( reloc_info, nrelo);

	convert_outname( &header);        
	wr_name( symbol_table, nname);

	wr_string( string_area, nchar);
}

wr_fatal()
{
	fprint( STDERR, "write failed\n");
	abort();
}


convert_outname( header)
struct outhead *header;
{
    int i;

	for ( i=0; i < nname; i++) {
		symbol_table[ i].on_foff += OFF_CHAR( *header);
    }
}


