#include <system.h>
#include <alloc.h>
#include <out.h>
#include "mach.h"
#include "back.h"

static reduce_name_table();
static convert_outname();

static int nrelo;

output_back()
/* Dump the tables.
 * Notice : entries in the symbol_table are converted.
 */

{
	struct outhead header;
	struct outsect sect;
        long	ntext = text - text_area, 
		ndata = data - data_area,
		nchar;

	nrelo = relo - reloc_info;

	reduce_name_table();

	nchar = string - string_area;
	header.oh_magic = O_MAGIC;
	header.oh_stamp = 0;
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

static
reduce_name_table()
{
	/*
	 * Reduce the name table size. This is done by first marking
	 * the name-table entries that are needed for relocation, then
	 * removing the entries that are compiler-generated and not
	 * needed for relocation, while remembering how many entries were
	 * removed at each point, and then updating the relocation info.
	 * After that, the string table is reduced.
	 */

#define S_NEEDED S_MOD
#define removable(nm)	(!(nm->on_type & (S_NEEDED|S_STB)) && *(nm->on_foff+string_area) == GENLAB)

	register int *diff_index =
		(int *) Malloc((unsigned)(nname + 1) * sizeof(int));
	register struct outrelo *rp = reloc_info;
	register struct outname *np;
	register int i;
	char *new_str;
	register char *p, *q;

	*diff_index++ = 0;
	for (i = 0; i < nrelo; i++) {
		if (symbol_table[rp->or_nami].on_valu == -1 ||
		    (symbol_table[rp->or_nami].on_type & S_COM)) {
			symbol_table[rp->or_nami].on_type |= S_NEEDED;
		}
		rp++;
	}

	for (i = 0, np = symbol_table; i < nname; i++, np++) {
		diff_index[i] = diff_index[i-1];
		if (removable(np)) {
			diff_index[i]++;
		}
		if ((np->on_type & S_TYP) == S_CRS) {
			struct outname *n = &symbol_table[(int) np->on_valu];
			if (! (n->on_type & S_COM)) {
				np->on_type &= ~S_TYP;
				np->on_type |= (n->on_type & S_TYP);
				np->on_valu = n->on_valu;
			}
		}
	}

	rp = reloc_info;
	for (i = 0; i < nrelo; i++) {
		symbol_table[rp->or_nami].on_type &= ~S_NEEDED;
		rp->or_nami -= diff_index[rp->or_nami];
		rp++;
	}
	for (i = 0, np = symbol_table; i < nname; i++, np++) {
		if ((np->on_type & S_TYP) == S_CRS) {
			np->on_valu -= diff_index[(int) np->on_valu];
		}
		if (diff_index[i] && diff_index[i] == diff_index[i-1]) {
			symbol_table[i - diff_index[i]] = *np;
		}
	}
	nname -= diff_index[nname - 1];

	free((char *)(diff_index-1));

	new_str = q = Malloc((unsigned)(string - string_area));
	for (i = 0, np = symbol_table; i < nname; i++, np++) {
		p = np->on_foff + string_area;
		np->on_foff = q - new_str;
		while (*q++ = *p) p++;
	}
	free(string_area);
	string_area = new_str;
	string = q;
	for (i = 0, np = symbol_table; i < nname; i++, np++) {
		if ((np->on_type & S_TYP) == S_CRS) {
			/* replace by reference to string */
			np->on_valu = symbol_table[(int) np->on_valu].on_foff;
		}
	}
}

wr_fatal()
{
	fprint( STDERR, "write failed\n");
	sys_stop(S_ABORT);
}


static
convert_outname( header)
struct outhead *header;
{
	int i;
	register struct outname *np;
	register long l = OFF_CHAR(*header);

	for (i = 0, np = symbol_table; i < nname; i++, np++) {
		np->on_foff += l;
		if ((np->on_type & S_TYP) == S_CRS) {
			np->on_valu += l;
		}
	}
}
