#include <em.h>
#include <out.h>
#include <system.h>
#include "mach.h"
#include "back.h"
#include "header.h"

/* There are two forms of relocation program counter relative or
 * absolute.
 */

#ifdef WORDS_REVERSED
#ifdef BYTES_REVERSED
#define RRR (RELO4|RELBR|RELWR)
#else
#define RRR (RELO4|RELWR)
#endif
#else
#ifdef BYTES_REVERSED
#define RRR (RELO4|RELBR)
#else
#define RRR (RELO4)
#endif
#endif

reloc4( sym, off, pcrel)
char *sym;
arith off;
int pcrel;
{
	register struct outrelo *r;

	if ( relo - reloc_info >= size_reloc)
 		mem_relo();

	r = relo;

	r->or_type = ( pcrel) ? RELPC|RRR : RRR;
	r->or_sect =  S_MIN + conv_seg( cur_seg);
	r->or_nami = find_sym(sym, REFERENCE);
	r->or_addr = cur_value();
	gen4( (pcrel) ? off - ( r->or_addr + 4) : off);

	/* print( "r %s r %ld s %d in %d adrr %ld off %ld\n",
	         sym, pcrel, cur_seg, r->or_nami, r->or_addr, 
		(pcrel) ? off-cur_value() : off);
	 */
	 
	relo++;
}
