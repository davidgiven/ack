#include <system.h>
#include <out.h>
#include "back.h"

/* Solve the local references.
 */

#define seg_index( s)	( nname - SEGBSS - 1 + s)

long 		get4(); 
extern short    get2();
extern char 	get1();


do_local_relocation()

/* Check if this reference is solvable. External references contain
 * -1 in 'on_valu'.
 */
{
	register struct outrelo *ptr;
	register int s;
	
	for ( ptr = reloc_info; ptr < relo; ptr++) {
		s = ptr->or_nami;
		if ( symbol_table[ s].on_valu  != -1) 
			do_relo(&symbol_table[ s], ptr);
	}
}


do_relo(np,rp)
struct outname *np;
struct outrelo *rp;

/* Solve the reference relative to the start of the segment where the symbol
 * is defined.
 */
{
	long oldval,newval;
	char *sect;

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

	if  ( rp->or_type & RELO4) {
		oldval = get4( sect, rp->or_addr);
		newval = oldval + np->on_valu;
		put4( sect, rp->or_addr, newval);

		rp->or_nami = seg_index(
			 ( symbol_table[ rp->or_nami].on_type & S_TYP) - S_MIN);
	}
	else if  ( rp->or_type & RELO2) {
		oldval = (long) get2( sect, rp->or_addr);
		newval = oldval + np->on_valu;
		put2( sect, rp->or_addr, (int) newval);

		rp->or_nami = seg_index(
			 ( symbol_table[ rp->or_nami].on_type & S_TYP) - S_MIN);
	}
	else if  ( rp->or_type & RELO1) {
		oldval = (long) get1( sect, rp->or_addr);
		newval = oldval + np->on_valu;
		put1( sect, rp->or_addr, (char) newval);

		rp->or_nami = seg_index(
			 ( symbol_table[ rp->or_nami].on_type & S_TYP) - S_MIN);
	}
	else
		print( STDERR, "do_relo() : bad relocation size\n");
	 /* print( 
		"reloc %s adrr=%ld sect=%ld oldval=%ld newval=%ld def = %ld\n",
		np->on_foff+string_area, rp->or_addr, rp->or_sect-S_MIN, oldval, 		newval, np->on_valu);
	  */
	 
}
