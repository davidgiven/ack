/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * This program converts ack.out format to XENIX386 x.out format.
 * It uses ~em/modules/lib/libobject.a.
 */

#include <stdio.h>

struct xexec {
	unsigned short	x_magic;
#define XMAGIC	01006
	unsigned short	x_ext;		/* 054 */
	long            x_text;
	long            x_data;
	long            x_bss;
	long		x_syms;
	long		x_reloc;	/* 0 */
	long            x_entry;	/* 0 */
	char		x_cpu;
#define XBSWAP	0x80
#define XWSWAP	0x40
#define X8086	0x04
#define X286	0x09
#define X386	0x0a
	char		x_relsym;	/* 0144, not used */
	unsigned short	x_renv;
#define XV5		0xc000
#define XVERS		0xc000		/* version mask */
#define XLOCK	 	0x1000
#define XSEG		0x0800		/* segment table present */
#define XPURE		0x0004		/* pure text */
#define XSEP		0x0002		/* separate I & D */
#define XEXEC		0x0001		/* executable */
};

struct xext {
	long		xe_trsize, xe_drsize, xe_tbase, xe_dbase; /* 0 */
	long		xe_stksize;
	long		xe_segpos;	/* 0140 ??? */
	long		xe_segsize;	/* 2 or 3 segments */
					/* one for name table, one for text+data
					   or one for text + one for data(-i)
					*/
	long		xe_mdtpos, xe_mdtsize; /* 0 */
	char		xe_mdttype;	/* 0 */
	char		xe_pagesize;	/* 2 */
	char		xe_ostype;	/* 1 */
	char		xe_osvers;	/* 2 */
	unsigned short	xe_eseg;	/* 077 ??? */
	unsigned short	xe_sres;	/* 0 */
};

struct xseg {
	unsigned short	xs_type;
#define XTEXT	1
#define XDATA	2
#define XTSYMS	3
	unsigned short	xs_attr;
#define XAMEM	0x8000
#define X_ABSS	0x0004
#define X_APURE	0x0008
#define X_A32B	0x0040
	unsigned short	xs_seg;		/* 077 for text, 0107 for data, 0 for sym */
	unsigned short	xs_sres;	/* 0 */
	long		xs_filpos;
	long		xs_psize;
	long		xs_vsize;
	long		xs_rbase;
	long		xs_lres;	/* 0 */
	long		xs_lres2;	/* 0 */
};

#include <out.h>

#ifndef NORCSID
static char rcs_id[] = "$Header$" ;
#endif

#define ENTRY 0x0L	/* entry point */

/*
 * Header and section table of new format object file.
 */
struct outhead	outhead;
struct outsect	outsect[S_MAX];

struct xexec exec;
struct xext  ext;
struct xseg  seg[3];

char	*output_file;
int	outputfile_created;
int	output;

char *program ;

extern long lseek();
#define TEXTSG	0
#define ROMSG	1
#define DATASG	2
#define BSSSG	3
#define LSECT	BSSSG+1
#define NSECT	LSECT+1

long	emit_symtab();

int	sep_id;

long
align(n, a)
	long n;
{
	return ((n + a - 1) / a) * a;
}

main(argc, argv)
	int	argc;
	char	*argv[];
{

	output = 1;
	program= argv[0] ;
	switch (argc) {
	case 1: rd_fdopen(0);
		break;
	case 3:	if ((output = creat(argv[2], 0644)) < 0) {
			fatal("Can't write %s.\n", argv[2]);
		}
		output_file = argv[2];
		outputfile_created = 1;
		/* FALLTHROUGH */
	case 2:
		if (! rd_open(argv[1]))
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s <ACK object> <Xenix object>.\n", argv[0]);
	}
	rd_ohead(&outhead);
	if (BADMAGIC(outhead))
		fatal("Not an ack object file.\n");
	if (outhead.oh_flags & HF_LINK)
		fatal("Contains unresolved references.\n");
	if (outhead.oh_nrelo > 0)
		fprintf(stderr, "Warning: relocation information present.\n");
	if ( outhead.oh_nsect!=LSECT && outhead.oh_nsect!=NSECT )
		fatal("Input file must have %d sections, not %ld\n",
			NSECT,outhead.oh_nsect) ;
	rd_sect(outsect, outhead.oh_nsect);
	while (outsect[TEXTSG].os_size % outsect[TEXTSG].os_lign) 
		outsect[TEXTSG].os_size++;
	while (outsect[ROMSG].os_size % outsect[ROMSG].os_lign) 
		outsect[ROMSG].os_size++;
	while (outsect[DATASG].os_size % outsect[DATASG].os_lign) 
		outsect[DATASG].os_size++;
	/* A few checks */
	if ( outsect[TEXTSG].os_base != ENTRY)
		fatal("text must start at 0x%lx not at 0x%lx\n", ENTRY,
			outsect[TEXTSG].os_base) ;
	if ( outsect[BSSSG].os_flen != 0 )
		fatal("bss space contains initialized data\n") ;
	if ( outsect[BSSSG].os_base != outsect[DATASG].os_base+
					outsect[DATASG].os_size )
		fatal("bss segment must follow data segment\n") ;

	exec.x_magic = XMAGIC;
	exec.x_ext = 054;
	exec.x_text = outsect[TEXTSG].os_size;
	exec.x_data = outsect[ROMSG].os_size + outsect[DATASG].os_size;
	exec.x_bss = outsect[BSSSG].os_size;
	exec.x_entry = outsect[TEXTSG].os_base;
	exec.x_cpu = XWSWAP | X386;
	exec.x_relsym = 0144;
	exec.x_renv = XV5 | XSEG | XLOCK | XEXEC;
	if ( outsect[ROMSG].os_base == 0x1880000 ) {
		/* Separate I/D */
		sep_id = 1;
		exec.x_renv |= XPURE | XSEP;
		if ( outsect[DATASG].os_base != outsect[ROMSG].os_base+
						outsect[ROMSG].os_size )
			fatal("data segment must follow rom\n") ;
	} else {
		if ( outsect[ROMSG].os_base != outsect[TEXTSG].os_base+
						outsect[TEXTSG].os_size )
			fatal("rom segment must follow text\n") ;
		if ( outsect[DATASG].os_base != outsect[ROMSG].os_base+
						outsect[ROMSG].os_size )
			fatal("data segment must follow rom\n") ;
	}
	if ( outhead.oh_nsect==NSECT ) {
		if ( outsect[LSECT].os_base != outsect[BSSSG].os_base+
						outsect[BSSSG].os_size )
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}
	ext.xe_stksize = 0;
	ext.xe_segpos = 0140;
	ext.xe_segsize = (2 + sep_id) * 040;
	ext.xe_pagesize = 2;
	ext.xe_ostype = 1;
	ext.xe_osvers = 2;
	ext.xe_eseg = 077;
	seg[0].xs_type = XDATA;
	seg[0].xs_attr = XAMEM | X_ABSS | X_A32B;
	seg[0].xs_seg = 0107;
	seg[0].xs_filpos = 02000;
	seg[1].xs_type = XTSYMS;
	seg[1].xs_attr = 1;	/* ??? */
	seg[1].xs_filpos = 02000 + outsect[TEXTSG].os_size +
			   outsect[DATASG].os_size + outsect[ROMSG].os_size;
	if (sep_id) {
		seg[2] = seg[1];
		seg[2].xs_filpos = 02000 + align(outsect[TEXTSG].os_size,02000) +
			   outsect[DATASG].os_size + outsect[ROMSG].os_size;
		seg[1] = seg[0];
		seg[1].xs_rbase = 0x1880000;
		seg[0].xs_type = XTEXT;
		seg[0].xs_attr = XAMEM | X_APURE | X_A32B;
		seg[0].xs_seg = 077;
		seg[0].xs_psize = seg[0].xs_vsize = outsect[TEXTSG].os_size;
		seg[1].xs_filpos = seg[0].xs_filpos + align(seg[0].xs_psize,02000);
		seg[1].xs_psize = outsect[ROMSG].os_size + outsect[DATASG].os_size;
		seg[1].xs_vsize = seg[1].xs_psize + outsect[BSSSG].os_size;
	}
	else {
		seg[0].xs_psize = outsect[TEXTSG].os_size +
				  outsect[ROMSG].os_size +
				  outsect[DATASG].os_size;
		seg[0].xs_vsize = seg[0].xs_psize + outsect[BSSSG].os_size;
	}
	lseek(output, seg[1+sep_id].xs_filpos, 0);
	if (! (exec.x_syms = seg[1+sep_id].xs_psize = emit_symtab())) {
		/* not enough memory to produce symbol table, do without */
		fprintf(stderr, "%s: warning: no symbol table produced\n", 
			program);
		ext.xe_segsize -= 040;
		seg[1+sep_id].xs_type = 0;
	}
	lseek(output, seg[0].xs_filpos, 0);

	emits(&outsect[TEXTSG]) ;
	if (sep_id) {
		lseek(output, seg[1].xs_filpos, 0);
	}
	emits(&outsect[ROMSG]) ;
	emits(&outsect[DATASG]) ;

	lseek(output, 0L, 0);
	header();

	if ( outputfile_created ) chmod(argv[2],0755);
	exit(0);
}

#define shortcvt(val, p) (*p++ = val, *p++ = val >> 8)
#define longcvt(val, p)	 (*p++ = val, *p++ = val >> 8, *p++ = val >> 16, *p++ = val >> 24)

char buf[0300];

header()
{
	register char *p = buf;
	register int i;

	shortcvt(exec.x_magic, p);
	shortcvt(exec.x_ext, p);
	longcvt(exec.x_text, p);
	longcvt(exec.x_data, p);
	longcvt(exec.x_bss, p);
	longcvt(exec.x_syms, p);
	longcvt(exec.x_reloc, p);
	longcvt(exec.x_entry, p);
	*p++ = exec.x_cpu;
	*p++ = exec.x_relsym;
	shortcvt(exec.x_renv, p);

	longcvt(ext.xe_trsize, p);
	longcvt(ext.xe_drsize, p);
	longcvt(ext.xe_tbase, p);
	longcvt(ext.xe_dbase, p);
	longcvt(ext.xe_stksize, p);
	longcvt(ext.xe_segpos, p);
	longcvt(ext.xe_segsize, p);
	longcvt(ext.xe_mdtpos, p);
	longcvt(ext.xe_mdtsize, p);
	*p++ = ext.xe_mdttype;
	*p++ = ext.xe_pagesize;
	*p++ = ext.xe_ostype;
	*p++ = ext.xe_osvers;
	shortcvt(ext.xe_eseg, p);
	shortcvt(ext.xe_sres, p);

	p = &buf[0140];
	for (i = 0; i <= 2 && seg[i].xs_type != 0; i++) {
		shortcvt(seg[i].xs_type, p);
		shortcvt(seg[i].xs_attr, p);
		shortcvt(seg[i].xs_seg, p);
		shortcvt(seg[i].xs_sres, p);
		longcvt(seg[i].xs_filpos, p);
		longcvt(seg[i].xs_psize, p);
		longcvt(seg[i].xs_vsize, p);
		longcvt(seg[i].xs_rbase, p);
		longcvt(seg[i].xs_lres, p);
		longcvt(seg[i].xs_lres2, p);
	}

	write(output, buf, 0140 + i * 040);
}

/*
 * Transfer the emitted byted from one file to another.
 */
emits(section) struct outsect *section ; {
	register long	n ;
	register int	blk;
	char		buffer[BUFSIZ];

	n= section->os_flen ;
	rd_outsect(section - outsect);
	while (n > 0) {
		blk = n > BUFSIZ ? BUFSIZ : n;
		rd_emit(buffer, (long) blk);
		write(output, buffer, blk);
		n -= blk;
	}
	if ((n = section->os_size - section->os_flen) > 0) {
		for (blk = BUFSIZ - 1; blk >= 0; blk--) {
			buffer[blk] = 0;
		}
		while (n > 0) {
			blk = n > BUFSIZ ? BUFSIZ : n;
			write(output, buffer, blk);
			n -= blk;
		}
	}
}

long
emit_symtab()
{
	register int i;
	struct xnm {
		unsigned short s_type, s_seg;
		long	s_value;
	} xnm;
	char *chars, *xname;
	struct outname *names;
	register char *xptr;
	extern char *malloc();
	long off = OFF_CHAR(outhead);
	register char *p;
	register struct outname *np;

	chars = malloc((unsigned)(outhead.oh_nchar));
	if (! chars) return 0;
	np = names = (struct outname *)
		malloc(outhead.oh_nname * sizeof(struct outname));
	if (! np) {
		free(chars);
		return 0;
	}
	xptr = malloc((unsigned)(outhead.oh_nchar) + 9 * outhead.oh_nname);
	if (! xptr) {
		free(chars);
		free((char *) np);
		return 0;
	}
	xname = xptr;
	rd_name(np, outhead.oh_nname);
	rd_string(chars,outhead.oh_nchar);
	for (i = 0; i < outhead.oh_nname; i++, np++) {
		xnm.s_seg = 077;
		if (np->on_type & S_STB) {
			xnm.s_seg = np->on_desc;
			xnm.s_type = np->on_type;
		}
		else switch(np->on_type & S_ETC) {
		case S_FIL:
		case S_MOD:
			xnm.s_type = 0x1f;
			break;
		case S_SCT:
			xnm.s_type = 0x8;
			if ((np->on_type & S_TYP) != S_MIN+TEXTSG) {
				xnm.s_seg = 0107;
			}
			break;
		default:
			switch(np->on_type & S_TYP) {
			case S_UND:
				xnm.s_type = 0;
				break;
			case S_ABS:
				xnm.s_type = 1;
				xnm.s_seg = 0107;
				break;
			case S_MIN + TEXTSG:
				xnm.s_type = 2;
				break;
			case S_MIN + ROMSG:
			case S_MIN + DATASG:
				xnm.s_type = 3;
				xnm.s_seg = 0107;
				break;
			case S_MIN + BSSSG:
			case S_MIN + LSECT:
				xnm.s_type = 4;
				xnm.s_seg = 0107;
				break;
			default:
				fprintf(stderr,"warning: unknown s_type: %d\n",
					(int)(np->on_type) & S_TYP);
			}
		}
		if (np->on_type & S_EXT) xnm.s_type |= 0x20;
		xnm.s_value = np->on_valu;
		shortcvt(xnm.s_type, xptr);
		shortcvt(xnm.s_seg, xptr);
		longcvt(xnm.s_value, xptr);
		if (np->on_foff == 0) {
			*xptr++ = '\0';
		}
		else {
			long l = np->on_foff - off;
			if (l < 0 || l >= outhead.oh_nchar) {
				fatal("bad on_off: %ld\n",l);
			}
			p = &chars[l];
			do {
				*xptr++ = *p;
			} while (*p++);
		}
	}
	write(output, xname, xptr - xname);
	free(xname);
	free((char *) names);
	free(chars);
	return xptr - xname;
}

/* VARARGS1 */
fatal(s, a1, a2)
	char	*s;
{
	fprintf(stderr,"%s: ",program) ;
	fprintf(stderr, s, a1, a2);
	if (outputfile_created)
		unlink(output_file);
	exit(1);
}

rd_fatal()
{
	fatal("read error\n");
}
