#include <out.h>
#include <em.h>
#include "mach.h"
#include "back.h"
#include "header.h"

reloc1( sym, off, pcrel)
char *sym;
arith off;
int pcrel;
{
	if ( relo - reloc_info >= size_reloc)
		mem_relo();

	relo->or_type = RELO1;
#ifdef BYTES_REVERSED			/* Nog optimaliseren?? */
	relo->or_type |= RELBR;
#endif
#ifdef WORDS_REVERSED
	relo->or_type |= RELWR;
#endif
	relo->or_type |= ( pcrel) ? RELPC : S_UND;
	relo->or_sect =  S_MIN + conv_seg( cur_seg);
	relo->or_nami = find_sym(sym, REFERENCE);
	relo->or_addr = cur_value();
	gen1( (pcrel) ? off - ( cur_value() + 1) : off);

	relo++;
}
