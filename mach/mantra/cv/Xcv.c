/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Convert Mantra object format to ACK object format
 */
#include <stdio.h>
#include <out.h>

#define	FMAGIC	0407
#define REG	06

#define	RTEXT	00
#define	RDATA	01
#define	RBSS	02
#define	REXT	03

#define RBYTE	00
#define RWORD	01
#define RLONG	02

struct bhdr {
	long	fmagic;
	long	tsize;
	long	dsize;
	long	bsize;
	long	ssize;
	long	rtsize;
	long	rdsize;
	long	entry;
};

struct reloc {
	char x;
#define setseg(p,s)	(p->x = (p->x & ~03) | s)
#define setsiz(p,s)	(p->x = (p->x & ~014) | (s << 2))
#define setdis(p,s)	(p->x = (p->x & ~020) | (s << 4))
#define getseg(p)	(p->x&03)
#define getsiz(p)	((p->x >> 2) & 03)
#define getdis(p)	((p->x >> 4) & 01)
	char	 relpad2;
	short rsymbol;
	long rpos;
};

#define	N_UNDF	0
#define	N_ABS	01
#define	N_TEXT	02
#define	N_DATA	03
#define	N_BSS	04
#define	N_TYPE	037
#define	N_REG	024
#define	N_FN	037
#define	N_EXT	040

#define ASSERT(x) switch (2) { case 0: case (x): ; }

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

struct bhdr bh;

#define TOT_HDRSIZE	(sizeof(struct bhdr))

FILE		*input;

struct outrelo *orelo = 0;
struct outname *oname = 0;
char *strings = 0;
char *txt = 0;
char *data = 0;

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
		if ((input = fopen(argv[1], "r")) == (FILE *)0)
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s <Mantra object> <ACK object>.\n", argv[0]);
	}
	if (! readf(&bh, sizeof(struct bhdr), 1)) rd_fatal();
	if (bh.fmagic != FMAGIC) fatal("bad magic number\n");
	outhead.oh_magic = O_MAGIC;
	outhead.oh_stamp = 0;
	outhead.oh_nsect = 4;
	outhead.oh_nrelo = (bh.rtsize + bh.rdsize) / sizeof(struct reloc);
	outhead.oh_flags = outhead.oh_nrelo ? HF_LINK : 0;
	outhead.oh_nemit = bh.tsize + bh.dsize;
	strings = malloc((unsigned) bh.ssize + 20);
	orelo = (struct outrelo *)
			malloc(outhead.oh_nrelo * sizeof(struct outrelo));
	oname = (struct outname *)
			malloc((unsigned) (bh.ssize / 6 + 3) * sizeof (struct outname));
	txt = malloc(bh.tsize);
	data = malloc(bh.dsize);
	if (!strings || !orelo || !oname || !txt || !data) {
		fatal("No memory\n");
	}
	p = &outsect[TEXTSG];
	p->os_base = 0; p->os_size = p->os_flen = bh.tsize;
	p->os_foff = OFF_EMIT(outhead); p->os_lign = 1;
	p = &outsect[ROMSG];
	p->os_base = p->os_size = p->os_flen = 0;
	p->os_foff = OFF_EMIT(outhead) + bh.tsize; p->os_lign = 1;
	p = &outsect[DATASG];
	p->os_base = 0; p->os_size = p->os_flen = bh.dsize;
	p->os_foff = OFF_EMIT(outhead) + bh.tsize; p->os_lign = 1;
	p = &outsect[BSSSG];
	p->os_base = p->os_flen = 0; p->os_size = bh.bsize;
	p->os_foff = OFF_EMIT(outhead)+bh.tsize+bh.dsize; p->os_lign = 1;

	if (bh.tsize && ! readf(txt, 1, bh.tsize)) rd_fatal();
	if (bh.dsize && ! readf(data, 1, bh.dsize)) rd_fatal();

	symtab(oname, strings);
	relo(orelo, txt, data);
	wr_ohead(&outhead);
	wr_sect(outsect, 4);
	wr_outsect(TEXTSG);
	wr_emit(txt, bh.tsize);
	wr_outsect(DATASG);
	wr_emit(data,bh.dsize);
	wr_relo(orelo, outhead.oh_nrelo);
	wr_name(oname, outhead.oh_nname);
	wr_string(strings, outhead.oh_nchar);
	wr_close();
	return 0;
}

symtab(name, string)
	struct outname *name;
	char *string;
{
	register struct outname *oname = name;
	register char *strings = string;
	register int c;
	unsigned nnames = 3;
	register long ssize = bh.ssize;
	register char *b;

	oname->on_valu = 0; oname->on_foff = 0; oname->on_desc = 0;
	oname->on_type = (S_MIN+TEXTSG) | S_SCT;
	b = ".text"; while (*strings++ = *b) b++;
	oname++;
	oname->on_valu = 0; oname->on_foff = strings - string; oname->on_desc = 0;
	oname->on_type = (S_MIN+DATASG) | S_SCT;
	b = ".data"; while (*strings++ = *b) b++;
	oname++;
	oname->on_valu = 0; oname->on_foff = strings - string; oname->on_desc = 0;
	oname->on_type = (S_MIN+BSSSG) | S_SCT;
	b = ".bss"; while (*strings++ = *b) b++;
	oname++;
	while (ssize > 0) {
		c = getc(input);
		getc(input);
		oname->on_valu = getw(input);
		ssize -= 6;
		switch(c & ~N_EXT) {
		case N_ABS:
			oname->on_type = S_ABS;
			break;
		case N_TEXT:
			oname->on_type = S_MIN + TEXTSG;
			break;
		case N_DATA:
			oname->on_type = S_MIN + DATASG;
			oname->on_valu -= bh.tsize;
			break;
		case N_BSS:
			oname->on_type = S_MIN + BSSSG;
			oname->on_valu -= bh.tsize + bh.dsize;
			break;
		case N_UNDF:
			if (! oname->on_valu) {
				oname->on_type = S_UND;
				break;
			}
			oname->on_type = (S_MIN + BSSSG) | S_COM;
			break;
		case REG:
		case N_REG:
		case N_FN:
			while (ssize > 0 && (getc(input) > 0))
				ssize--;
			continue;
		default:
			fatal("Illegal type field %d in namelist\n", c);
		}
		if (c & N_EXT) oname->on_type |= S_EXT;
		oname->on_desc = 0;
		oname->on_foff = strings - string;
		nnames++;
		while (ssize > 0 && (c = getc(input)) > 0) {
			*strings++ = c;
			ssize--;
		}
		ssize--;
		*strings++ = c;
		oname++;
	}
	outhead.oh_nname = nnames;
	outhead.oh_nchar = strings - string;
	oname = name;
	ssize = OFF_CHAR(outhead);
	while (nnames--) {
		oname->on_foff += ssize;
		oname++;
	}
}

long
get4(c)
	register char *c;
{
	register long l = (long) (*c++ & 0377) << 24;

	l |= (long) (*c++ & 0377) << 16;
	l |= (long) (*c++ & 0377) << 8;
	l |= (long) (*c++ & 0377);
	return l;
}

put4(l, c)
	register char *c;
	register long l;
{
	*c++ = (l >> 24);
	*c++ = (l >> 16);
	*c++ = (l >> 8);
	*c = l;
}

relo(orelo, txt, data)
	struct outrelo *orelo;
	char *txt, *data;
{
	register struct outrelo *relo = orelo;
	register relocnt = outhead.oh_nrelo;
	struct reloc rel;
	register struct reloc *relp = &rel;
	int ndat = bh.rdsize / sizeof(struct reloc);	

	while (relocnt-- > 0) {
		readf(relp, sizeof(struct reloc), 1);
		relo->or_sect = (relocnt >= ndat) ? S_MIN + TEXTSG :
						     S_MIN + DATASG;
		relo->or_addr = relp->rpos;
		relo->or_nami = relp->rsymbol + 3;
		switch(getseg(relp)) {
		case RTEXT:
			relo->or_nami = 0;
			break;
		case RDATA:
			relo->or_nami = 1;
			break;
		case RBSS:
			relo->or_nami = 2;
			break;
		case REXT:
			break;
		default:
			fatal("Error 1 in relocation\n");
		}
		switch(getsiz(relp)) {
		case RBYTE:
			relo->or_type = RELO1|RELBR|RELWR;
			break;
		case RWORD:
			relo->or_type = RELO2|RELBR|RELWR;
			break;
		case RLONG: {
			register char *sct = (relocnt >= ndat ? txt : data) +
						relo->or_addr;
			long x;

			relo->or_type = RELO4|RELBR|RELWR;
			switch((oname + relo->or_nami)->on_type & S_TYP) {
			case (S_MIN+DATASG):
				x = get4(sct) - bh.tsize;

				put4(x, sct);
				break;
			case (S_MIN+BSSSG):
				if (! ((oname+relo->or_nami)->on_type & S_COM))
					x = get4(sct) - (bh.tsize + bh.dsize);
				put4(x, sct);
			}
			break;
			}
		default:
			fatal("Error 2 in relocation\n");
		}
		if (getdis(relp)) relo->or_type |= RELPC;
		relo++;
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
