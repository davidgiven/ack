#include <system.h>
#include <out.h>
#include "back.h"
#include "data.h"

/* Solve the local references.
 */

#define seg_index( s)	( nname - SEGBSS - 1 + s)

long 		get4(); 

do_local_relocation()

/* Check if this reference is solvable. External references contain
 * -1 in 'on_valu'.
 * Also remove useless relocation structures.
 */
{
	register struct outrelo *rp;
	int diff = 0;
	
	for ( rp = reloc_info; rp < relo; rp++) {
		register struct outname *np = &symbol_table[rp->or_nami];
		int olddiff = diff;
		
		if ((np->on_type & S_COM) && ! (np->on_type & S_EXT)) {
			long sz = np->on_valu;

			switchseg(SEGBSS);
			align_word();
			np->on_type &= (~S_COM);
			np->on_valu = cur_value();
			bss(sz);
		}
		if ( np->on_valu  != -1 && ! (np->on_type & S_COM)) {
			register long oldval,newval;
			register char *sect;

			switch( rp->or_sect - S_MIN) {
				case SEGTXT:
					sect = text_area;
					if ((rp->or_type & RELPC) &&
					    (np->on_type & S_TYP) - S_MIN == SEGTXT) {
						diff++;
					}
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

			if  ( rp->or_type & RELO4) {
				oldval = get4( sect, rp->or_addr);
				newval = oldval + np->on_valu;
				put4( sect, rp->or_addr, newval);
			}
			else if  ( rp->or_type & RELO2) {
				oldval = (long) get2( sect, rp->or_addr);
				newval = oldval + np->on_valu;
				put2( sect, rp->or_addr, (int) newval);
			}
			else if  ( rp->or_type & RELO1) {
				oldval = (long) get1( sect, rp->or_addr);
				newval = oldval + np->on_valu;
				put1( sect, rp->or_addr, (char) newval);
			}
			else
				fprint( STDERR, "do_relo() : bad relocation size\n");
			rp->or_nami = seg_index((np->on_type & S_TYP) - S_MIN);
	 		/* print( 
				"reloc %s adrr=%ld sect=%ld oldval=%ld newval=%ld def = %ld\n",
				np->on_foff+string_area, rp->or_addr, rp->or_sect-S_MIN, oldval, 		newval, np->on_valu);
	  		*/
		}
		if (diff && diff == olddiff) {
			rp[-diff] = rp[0];
		}
	}
	relo -= diff;
}
