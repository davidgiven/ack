#include <system.h>
#include <out.h>
#include "back.h"

/* Written to run on SUN, and generate code for SUN */

extern long 	_base_address[];

do_local_relocation()
{
	register struct outrelo *rp;
	
	/* print( "n relocation records %d\n", relo - reloc_info);  */

	_base_address[SEGTXT] = 0;
	_base_address[SEGCON] = text - text_area;
	_base_address[SEGBSS] = _base_address[SEGCON] + data - data_area;
	for ( rp = reloc_info; rp < relo; rp++) {
		register struct outname *np = &symbol_table[rp->or_nami];

		if ( np->on_valu  != -1 && ! (np->on_type & S_COM)) {
			register char *sect;

			switch( rp->or_sect - S_MIN) {
				case SEGTXT:
					sect = text_area;
					break;
				case SEGCON:
					sect = data_area;
					break;
				default:
					fprint( STDERR, 
					  "do_local_relo(): bad section %d\n",
				 		rp->or_sect - S_MIN);
					break;
			}
			if  ( rp->or_type & RELO4) 
				*((long *)(sect+rp->or_addr)) +=
					np->on_valu +
					_base_address[(np->on_type&S_TYP)-S_MIN];
			else
				fprint( STDERR,
				  "do_relo() : bad relocation size\n");
		}
	}
}
