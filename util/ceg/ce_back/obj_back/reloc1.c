#include <out.h>
#include "mach.h"
#include "back.h"
#include "header.h"

/* There are two forms of relocation program counter relative or
 * absolute.
 */

#ifdef WORDS_REVERSED
#ifdef BYTES_REVERSED
#define RRR (RELO1|RELBR|RELWR)
#else
#define RRR (RELO1|RELWR)
#endif
#else
#ifdef BYTES_REVERSED
#define RRR (RELO1|RELBR)
#else
#define RRR (RELO1)
#endif
#endif

reloc1( sym, off, pcrel)
char *sym;
arith off;
int pcrel;
{
	register struct outrelo *r;

	if ( relo - reloc_info >= size_reloc)
		mem_relo();

	r = relo;
	r->or_type |= ( pcrel) ? RELPC|RRR : RRR;
	r->or_sect =  S_MIN + conv_seg( cur_seg);
	r->or_nami = find_sym(sym, REFERENCE);
	r->or_addr = cur_value();
	gen1( (pcrel) ? off - ( r->or_addr + 1) : off);

	relo++;
}
