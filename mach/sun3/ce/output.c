#include <system.h>
#include <em.h>
#include <out.h>
#include "mach.h"
#include "back.h"
#include "data.h"

/*	Unportable code. Written for SUN, meant to be run on a SUN.
*/
#ifndef sun
Read above comment ...
#endif

extern File *out_file;

#include <a.out.h>
#include <alloc.h>

struct exec u_header;

long ntext, ndata, nrelo, nchar, base_address();
int trsize=0, drsize=0;

output()
{
	register int i;
	struct relocation_info *u_reloc;
	struct nlist *u_name;

	/*
	 * first, convert relocation data structures. This also requires
	 * some re-ordering, as SUN .o format needs has text relocation
	 * structures in front of the data relocation structures, whereas in
	 * ACK they can be in any order.
	 */

	nrelo = relo - reloc_info;
	u_reloc = (struct relocation_info *)
			Malloc((unsigned)nrelo*sizeof(struct relocation_info));

	for (i = 0; i < nrelo; i++) {
		if (  ( reloc_info[i].or_sect-S_MIN) == SEGTXT) {
			convert_reloc( &reloc_info[i], u_reloc++);
			trsize++;
		}
	}
	for (i = 0; i < nrelo; i++) {
		if (  ( reloc_info[i].or_sect-S_MIN) != SEGTXT) {
			convert_reloc( &reloc_info[i], u_reloc++);
			drsize++;
		}
	}

	u_reloc -= nrelo;

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


init_unixheader()
{
	ntext = text - text_area;
	ndata = data - data_area;
	nchar = string - string_area;

	u_header.a_magic = OMAGIC;
	u_header.a_machtype = M_68020;
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

convert_reloc( a_relo, u_relo)
struct outrelo *a_relo;
struct relocation_info *u_relo;
{
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
				      break;
			case SEGCON : u_relo->r_symbolnum = N_DATA;
				      break;
			case SEGBSS : u_relo->r_symbolnum = N_BSS;
				      break;
			default : fprint( STDERR, 
					   "convert_relo(): bad segment %d\n",
			    (symbol_table[ a_relo->or_nami].on_type & S_TYP) - S_MIN);
		}
	}
}


/*
int convert_length( length)
int length;
{
	if ( length & RELO1)
		return( 0);
	else if ( length & RELO2)
		return( 1);
	else if ( length & RELO4)
		return( 2);
	else
		fprint( STDERR, "convert_length(): size is impossible %d\n",
			length);
}
*/

#define 	n_mptr 		n_un.n_name
#define 	n_str		n_un.n_strx


convert_name( a_name, u_name)
struct outname *a_name;
struct nlist *u_name;
{
	/* print( "naam is %s\n", a_name->on_foff + string_area);   */

	u_name->n_str = a_name->on_foff + 4;
	fill_type( &(u_name->n_type), &(a_name->on_type), a_name->on_valu);
	u_name->n_other = '\0';
	u_name->n_desc = 0;
	if (a_name->on_type & S_COM) 
		u_name->n_value = a_name->on_valu;
	else if ( a_name->on_valu != -1)
		u_name->n_value = a_name->on_valu + 
			base_address( ( a_name->on_type & S_TYP) - S_MIN);
	else 
		 u_name->n_value = 0;
}


fill_type( u_type, a_type, valu)
unsigned char *u_type;
ushort *a_type;
long valu; 
{
	int sect;

	*u_type = '\0';
	*u_type |= ( *a_type & S_EXT) ? N_EXT : N_UNDF;

	if ( valu != -1 && (! (*a_type & S_COM))) {
		sect = ( *a_type & S_TYP ) - S_MIN;
		switch ( sect) {
			case SEGTXT: *u_type |= N_TEXT;
			      	     break;
			case SEGCON : *u_type |= N_DATA;
			              break;
			case SEGBSS : *u_type |= N_BSS;
			              break;
			default: fprint(STDERR, 
					"fill_type() : bad section %d\n", sect);
		}
	}
}


long base_address( seg)
int seg;
{
	switch ( seg) {
		case SEGTXT : return( 0);
			      break;
		case SEGCON : return( text - text_area);
			      break;
		case SEGBSS : return( text - text_area + data - data_area);
			      break;
		default : fprint( STDERR,
			"base_adres() : bad section %d\n", seg);
	}
}

put_stringtablesize( n)
long n;
{
	putbuf( (char *)&n, 4L);
}


putbuf(buf,n)
char *buf;
long n;
{
	sys_write( out_file, buf, n);
}
