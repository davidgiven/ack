#include <system.h>
#include <out.h>
#include "data.h"
#include "back.h"

long 		get4(); 
extern long 	base_address();
extern short    get2();
extern char 	get1();

static do_relo();

do_local_relocation()
{
	register struct outrelo *ptr;
	register int s;
	
	/* print( "n relocation records %d\n", relo - reloc_info);  */

	for ( ptr = reloc_info; ptr < relo; ptr++) {
		s = ptr->or_nami;
		if ( symbol_table[ s].on_valu  != -1 &&
		     ! (symbol_table[s].on_type & S_COM)) 
			do_relo(&symbol_table[ s], ptr);
	}
}


static
do_relo(np,rp)
struct outname *np;
struct outrelo *rp;
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
		print( STDERR, "do_relo() : bad relocation size\n");
	/* print( "reloc %s adrr=%ld sect=%ld oldval=%ld newval=%ld def = %ld\n",
np->on_foff + string_area, rp->or_addr, rp->or_sect-S_MIN, oldval, newval, 
np->on_valu);
	 */
}
