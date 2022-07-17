/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Convert SUN-3 object format to ACK object format. Only on Sun-3
 */
#include <stdio.h>
#include <out.h>
#include <a.out.h>
	
/*
 * Header and section table of new format object file.
 */
struct outhead	outhead;
struct outsect	outsect[4];

#define TEXTSG 0
#define ROMSG 1
#define DATASG 2
#define BSSSG 3

char	*output_file;

char *program ;

char flag ;

#define readf(a, b, c)	fread((a), (b), (int)(c), input)

/* Output file definitions and such */

struct exec bh;

FILE		*input;

struct outname *oname = 0;
char *strings = 0;
char *txt = 0;
char *data = 0;
unsigned nstrings;

main(argc, argv)
	int	argc;
	char	*argv[];
{
	register struct outsect *p;
	char *malloc();

	input = stdin;
	program= argv[0] ;
	if ( argc>1 && argv[1][0]=='-' ) {
		flag=argv[1][1] ;
		argc-- ; argv++ ;
	}
	switch (argc) {
	case 3:	if (! wr_open(argv[2]))
			fatal("Can't write %s.\n", argv[2]);
		output_file = argv[2];
		if ((input = fopen(argv[1], "rb")) == (FILE *)0)
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s <Sun-3 object> <ACK object>.\n", argv[0]);
	}
	if (! readf(&bh, sizeof(struct exec), 1)) rd_fatal();
	if (N_BADMAG(bh)) fatal("bad magic number\n");
	outhead.oh_magic = O_MAGIC;
	outhead.oh_stamp = 0;
	outhead.oh_nsect = 4;
	outhead.oh_nname = 3 + bh.a_syms / sizeof(struct nlist);
	outhead.oh_nrelo = (bh.a_trsize + bh.a_drsize) / sizeof(struct reloc_info_68k);
	outhead.oh_flags = outhead.oh_nrelo ? HF_LINK : 0;
	outhead.oh_nemit = bh.a_text + bh.a_data - (bh.a_magic == ZMAGIC ? sizeof(struct exec) : 0);
	fseek(input, N_STROFF(bh), 0);
	nstrings = getw(input);
	strings = malloc(nstrings + 20);
	oname = (struct outname *)
			malloc((unsigned) (3 + bh.a_syms / sizeof(struct nlist)) * sizeof (struct outname));
	txt = malloc(bh.a_text);
	data = malloc(bh.a_data);
	if (!strings || !oname || !txt || !data) {
		fatal("No memory\n");
	}
	if (! readf(strings, nstrings, 1)) rd_fatal();
	p = &outsect[TEXTSG];
	p->os_base = N_TXTADDR(bh); p->os_size = p->os_flen = bh.a_text;
	if (bh.a_magic == ZMAGIC) {
		p->os_base += sizeof(struct exec);
		p->os_size -= sizeof(struct exec);
		p->os_flen -= sizeof(struct exec);
		p->os_foff = OFF_EMIT(outhead); p->os_lign = 1;
	}
	p = &outsect[ROMSG];
	p->os_base = (p-1)->os_base + (p-1)->os_size;
	p->os_size = p->os_flen = 0;
	p->os_foff = OFF_EMIT(outhead) + bh.a_text; p->os_lign = 1;
	p = &outsect[DATASG];
	p->os_base = N_DATADDR(bh); p->os_size = p->os_flen = bh.a_data;
	p->os_foff = OFF_EMIT(outhead) + (p-2)->os_flen;
	p->os_lign = bh.a_magic == ZMAGIC ? 0x20000 : 1;
	p = &outsect[BSSSG];
	p->os_base = N_BSSADDR(bh);
	p->os_flen = 0; p->os_size = bh.a_bss;
	p->os_foff = OFF_EMIT(outhead)+(p-1)->os_foff + (p-1)->os_flen;
	p->os_lign = 1;

	if (bh.a_magic == ZMAGIC) bh.a_text -= sizeof(struct exec);
	fseek(input, sizeof(struct exec), 0);
	if (bh.a_text && ! readf(txt, 1, bh.a_text)) rd_fatal();
	if (bh.a_data && ! readf(data, 1, bh.a_data)) rd_fatal();

	symtab(oname, strings);
	wr_ohead(&outhead);
	wr_sect(outsect, 4);
	wr_outsect(TEXTSG);
	wr_emit(txt, bh.a_text);
	wr_outsect(DATASG);
	wr_emit(data,bh.a_data);
	wr_name(oname, outhead.oh_nname);
	wr_string(strings, outhead.oh_nchar);
	wr_close();
	exit(0);
}

symtab(name, string)
	struct outname *name;
	char *string;
{
	register struct outname *oname = name;
	register char *strings = string+nstrings;
	register int c;
	register char *b;
	struct nlist n;

	oname->on_valu = 0; oname->on_foff = strings - string + OFF_CHAR(outhead); oname->on_desc = 0;
	oname->on_type = (S_MIN+TEXTSG) | S_SCT;
	b = ".text"; while (*strings++ = *b) b++;
	oname++;
	oname->on_valu = 0; oname->on_foff = strings - string + OFF_CHAR(outhead); oname->on_desc = 0;
	oname->on_type = (S_MIN+DATASG) | S_SCT;
	b = ".data"; while (*strings++ = *b) b++;
	oname++;
	oname->on_valu = 0; oname->on_foff = strings - string + OFF_CHAR(outhead); oname->on_desc = 0;
	oname->on_type = (S_MIN+BSSSG) | S_SCT;
	b = ".bss"; while (*strings++ = *b) b++;
	oname++;
	outhead.oh_nchar = strings - string;
	while (bh.a_syms > 0) {
		if (! readf(&n, sizeof(struct nlist), 1)) rd_fatal();
		bh.a_syms -= sizeof(struct nlist);
		oname->on_desc = n.n_desc;
		if (n.n_un.n_strx - 4 < 0) oname->on_foff = 0;
		else oname->on_foff = OFF_CHAR(outhead) - 4 + n.n_un.n_strx;
		oname->on_valu = n.n_value;

		if (n.n_type & N_STAB) {
			oname->on_type = n.n_type << 8;
			oname++;
			continue;
		}
		switch(n.n_type & ~N_EXT) {
		case N_ABS:
			oname->on_type = S_ABS;
			break;
		case N_TEXT:
			oname->on_type = S_MIN + TEXTSG;
			break;
		case N_DATA:
			oname->on_type = S_MIN + DATASG;
			oname->on_valu -= bh.a_text;
			break;
		case N_BSS:
			oname->on_type = S_MIN + BSSSG;
			oname->on_valu -= bh.a_text + bh.a_data;
			break;
		case N_UNDF:
			if (! oname->on_valu) {
				oname->on_type = S_UND;
				break;
			}
			oname->on_type = (S_MIN + BSSSG) | S_COM;
			break;
		case N_FN:
			oname->on_type = S_FIL;
			break;
		default:
			fatal("Illegal type field %d in namelist\n", c);
		}
		if (n.n_type & N_EXT) oname->on_type |= S_EXT;
		oname++;
	}
}

/* VARARGS1 */
fatal(s, a1, a2)
	char	*s;
{
	fprintf(stderr,"%s: ",program) ;
	fprintf(stderr, s, a1, a2);
	unlink(output_file);
	exit(-1);
}

wr_fatal() { fatal("Write error\n"); }
rd_fatal() { fatal("Read error\n"); }
