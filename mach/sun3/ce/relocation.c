#include <system.h>
#include <out.h>
#include "data.h"
#include "back.h"

long 		get4(); 
extern long 	base_address();
/*
extern short    get2();
extern char 	get1();
*/

do_local_relocation()
{
	register struct outrelo *rp;
	
	/* print( "n relocation records %d\n", relo - reloc_info);  */

	for ( rp = reloc_info; rp < relo; rp++) {
		register struct outname *np = &symbol_table[rp->or_nami];

		if ( np->on_valu  != -1 && ! (np->on_type & S_COM)) {
			register long oldval,newval;
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
			oldval = get4( sect, rp->or_addr);
			newval = oldval + np->on_valu + 
				base_address( (np->on_type & S_TYP) -S_MIN);	 
			if  ( rp->or_type & RELO4) 
				put4( sect, rp->or_addr, newval);
			/*
			else if  ( rp->or_type & RELO2) 
				put2( sect, rp->or_addr, (int) newval);
			else if  ( rp->or_type & RELO1) 
				put1( sect, rp->or_addr, (char) newval);
			*/
			else
				print( STDERR,
				  "do_relo() : bad relocation size\n");
		}
	}
}
