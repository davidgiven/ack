#include <system.h>
#include <em.h>
#include <out.h>
#include "mach.h"
#include "back.h"

/*	Unportable code. Written for VAX, meant to be run on a VAX.
*/
#ifndef vax
Read above comment ...
#endif

extern File *B_out_file;

#include <a.out.h>
#include <alloc.h>

static struct exec u_header;

static long ntext, ndata, nrelo, nchar;

long B_base_address[SEGBSS+1];

static int trsize=0, drsize=0;

static struct relocation_info *u_reloc;

static reduce_name_table(), putbuf(), put_stringtablesize();
static convert_name(), convert_reloc(), init_unixheader();

output_back()
{
	register int i;
	register struct nlist *u_name;
	register struct outrelo *rp;

	/*
	 * Convert relocation data structures. This also requires
	 * some re-ordering, as SUN .o format needs has text relocation
	 * structures in front of the data relocation structures, whereas in
	 * ACK they can be in any order.
	 */

	nrelo = relo - reloc_info;
	u_reloc = (struct relocation_info *)
			Malloc((unsigned)nrelo*sizeof(struct relocation_info));

	rp = reloc_info;
	for (i = nrelo; i > 0; i--, rp++) {
		if (  ( rp->or_sect-S_MIN) == SEGTXT &&
			convert_reloc( rp, u_reloc)) {
			trsize++;
			u_reloc++;
		}
	}
	rp = reloc_info;
	for (i = nrelo; i > 0; i--, rp++) {
		if (  ( rp->or_sect-S_MIN) != SEGTXT &&
			convert_reloc( rp, u_reloc)) {
			u_reloc++;
			drsize++;
		}
	}

	nrelo = trsize + drsize;
	u_reloc -= nrelo;

	reduce_name_table();

	init_unixheader();

	putbuf( (char *) &u_header, sizeof(struct exec));
	putbuf( (char *) text_area,  ntext);
	putbuf( (char *) data_area, ndata);
	putbuf((char *) u_reloc, sizeof(struct relocation_info)*nrelo);
	free(u_reloc);
	
	u_name = (struct nlist *)
			Malloc((unsigned)nname * sizeof(struct nlist));

	for (i = 0; i < nname ; i++) { /* The segment names can be omitted */
		convert_name( &symbol_table[i], u_name++);
	}
	u_name -= nname;
	putbuf((char *) u_name, sizeof(struct nlist)*nname);
	free(u_name);

	/* print( "size string_area %d\n", nchar); */

	put_stringtablesize( nchar + 4);
	putbuf((char *) string_area, nchar);
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

#define S_NEEDED	S_MOD
#define removable(nm)	(!(nm->on_type & (S_NEEDED|S_STB)) && *(nm->on_foff+string_area) == GENLAB)

	register int *diff_index =
		(int *) Malloc((unsigned)(nname + 1) * sizeof(int));
	register int i;
	register struct outname *np;
	char *new_str;
	register char *p, *q;
	register struct relocation_info *rp;

	*diff_index++ = 0;
	rp = u_reloc;
	for (i = nrelo; i > 0; i--, rp++) {
		if (rp->r_extern) {
			symbol_table[rp->r_symbolnum].on_type |= S_NEEDED;
		}
	}

	np = symbol_table;
	for (i = 0; i < nname; i++, np++) {
		int old_diff_index = diff_index[i-1];

		if (removable(np)) {
			diff_index[i] = old_diff_index + 1;
		}
		else {
			diff_index[i] = old_diff_index;
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

	rp = u_reloc;
	for (i = nrelo; i > 0; i--, rp++) {
		if (rp->r_extern) {
			symbol_table[rp->r_symbolnum].on_type &= ~S_NEEDED;
			rp->r_symbolnum -= diff_index[rp->r_symbolnum];
		}
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
	np = symbol_table;
	for (i = nname; i > 0; i--, np++) {
		p = np->on_foff + string_area;
		np->on_foff = q - new_str;
		while (*q++ = *p) p++;
	}
	free(string_area);
	string_area = new_str;
	string = q;
}

static
init_unixheader()
{
	ntext = text - text_area;
	ndata = data - data_area;
	nchar = string - string_area;

	u_header.a_magic = OMAGIC;
	u_header.a_text = ntext;
	u_header.a_data = ndata;
	u_header.a_bss = nbss;
	u_header.a_syms = nname * sizeof(struct nlist);
	u_header.a_entry = 0;
	u_header.a_trsize = trsize * sizeof(struct relocation_info);
  	u_header.a_drsize = drsize * sizeof(struct relocation_info);
	/* print( "header %o %d %d %d %d %d %d %d\n",
		u_header.a_magic, u_header.a_text, u_header.a_data,
		u_header.a_bss, u_header.a_syms, u_header.a_entry,
		u_header.a_trsize, u_header.a_drsize);
	 */
}

static
convert_reloc( a_relo, u_relo)
register struct outrelo *a_relo;
register struct relocation_info *u_relo;
{
	int retval = 1;

	u_relo->r_address = a_relo->or_addr;
	u_relo->r_symbolnum = a_relo->or_nami;
	u_relo->r_pcrel = (a_relo->or_type & RELPC) >> 3;
	u_relo->r_length = 2;
	if ( symbol_table[ a_relo->or_nami].on_valu == -1 ||
	     (symbol_table[ a_relo->or_nami].on_type & S_COM)) 
		u_relo->r_extern = 1;
	else  
		u_relo->r_extern = 0;
	if ( u_relo->r_extern == 0) {
		switch ( (symbol_table[ a_relo->or_nami].on_type & S_TYP) - S_MIN) {
			case SEGTXT : u_relo->r_symbolnum = N_TEXT;
				      if (u_relo->r_pcrel &&
					  (a_relo->or_sect-S_MIN == SEGTXT))
						retval = 0;
				      break;
			case SEGCON : u_relo->r_symbolnum = N_DATA;
				      break;
			case SEGBSS : u_relo->r_symbolnum = N_BSS;
				      break;
/* Shut up; this can actually happen on erroneous input
			default : fprint( STDERR, 
					   "convert_relo(): bad segment %d\n",
			    (symbol_table[ a_relo->or_nami].on_type & S_TYP) - S_MIN);
*/
		}
	}
	return retval;
}

#define 	n_mptr 		n_un.n_name
#define 	n_str		n_un.n_strx

static
convert_name( a_name, u_name)
register struct outname *a_name;
register struct nlist *u_name;
{
	/* print( "naam is %s\n", a_name->on_foff + string_area);   */

	u_name->n_str = a_name->on_foff + 4;
	if (a_name->on_type & S_STB) u_name->n_type = a_name->on_type >> 8;
	else	u_name->n_type = 0;
	if ((a_name->on_type & S_TYP) == S_CRS) {
		a_name->on_valu = 0;
		a_name->on_type = S_COM;
	}
	if (a_name->on_valu != -1 && (! (a_name->on_type & S_COM))) {
		switch((a_name->on_type & S_TYP) - S_MIN) {
		case SEGTXT:
			u_name->n_type |= N_TEXT;
			break;
		case SEGCON:
			u_name->n_type |= N_DATA;
			break;
		case SEGBSS:
			u_name->n_type |= N_BSS;
			break;
/* Shut up; this can actually happen on erroneous input
		default:
			fprint(STDERR, "convert_name(): bad section %d\n",
				(a_name->on_type & S_TYP) - S_MIN);
			break;
*/
		}
	}
	if ((a_name->on_type & S_TYP) == S_UND ||
	    (a_name->on_type & S_EXT)) u_name->n_type |= N_EXT;
	u_name->n_other = '\0';
	u_name->n_desc = a_name->on_desc;
	if (a_name->on_type & S_COM) 
		u_name->n_value = a_name->on_valu;
	else if ( a_name->on_valu != -1)
		u_name->n_value = a_name->on_valu + 
			B_base_address[( a_name->on_type & S_TYP) - S_MIN];
	else 
		 u_name->n_value = 0;
}

static
put_stringtablesize( n)
long n;
{
	putbuf( (char *)&n, 4L);
}

static
putbuf(buf,n)
char *buf;
long n;
{
	sys_write( B_out_file, buf, n);
}
