/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Convert ACK a.out file to VAX Berkeley Unix object format.
 */

#include <stdio.h>
#include <out.h>
#include <assert.h>

long	lseek();

#define	OMAGIC	0407	/* old-fashioned */
#define	NMAGIC	0410	/* text write protexted */
#define ZMAGIC	0413	/* demand paging */

struct bhdr {
	long	magic;
	long	tsize;
	long	dsize;
	long	bsize;
	long	ssize;
	long	entry;
	long	rtsize;
	long	rdsize;
};

struct machrelo {
	long	address;
	long	relodata;
};
#define setpcrel(X,f)	(X |= (f<<7))
#define setsymbolnum(X,n)	(X = (X & 0377) | ((long)n << 8))
#define setextern(X,f)	(X |= (f << 4))
#define setlength(X,l)	(X = (X & ~0x60)|((long) l << 5))

struct sym {
	long	name;
	char	type;
	char	other;
	short	desc;
	long	value;
};

#define N_UNDF	0
#define	N_ABS	02
#define	N_TEXT	04
#define	N_DATA	06
#define	N_BSS	010
#define	N_EXT	01
#define N_FN	0x1f

/*
 * Header and section table of new format object file.
 */
struct outhead	outhead;
struct outsect	outsect[S_MAX];

char	*output_file;
int	outputfile_created;
long magic;

int rom_in_data;

char *program ;

char flag ;

/* Output file definitions and such */

struct bhdr bh;

#define ENTRY 0x0
#define TOT_HDRSIZE	(sizeof(struct bhdr))



#define TEXTSG	0
#define ROMSG	1
#define DATASG	2
#define BSSSG	3
#define LSECT	BSSSG+1
#define NSECT	LSECT+1

int		output;

int	unresolved;
int	nflag;
long	textsize ; 
long	datasize ;
long	bsssize;

long align(a,b)
	long a,b;
{
	a += b - 1;
	return a - a % b;
}

int
follows(pa, pb)
	register struct outsect *pa, *pb;
{
	/* return 1 if pa follows pb */

	return pa->os_base == align(pb->os_base+pb->os_size, pa->os_lign);
}

main(argc, argv)
	int	argc;
	char	*argv[];
{
	register int		nsect;

	program= argv[0] ;
	while ( argc>1 && argv[1][0]=='-' ) {
		flag=argv[1][1] ;
		if (flag == 'u') unresolved++;
		else if (flag == 'n') nflag = 1;
		argc-- ; argv++ ;
	}
	switch (argc) {
	case 3:	if ((output = creat(argv[2], 0644)) < 0 ||
		    (close(output), output = open(argv[2],2)) < 0)
			fatal("Can't write %s.\n", argv[2]);
		output_file = argv[2];
		outputfile_created = 1;
		if (! rd_open(argv[1]))
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s [-u] [-n] <ACK object> <Vax object>.\n", program);
	}
	rd_ohead(&outhead);
	if (BADMAGIC(outhead))
		fatal("Not an ack object file.\n");
	if (outhead.oh_flags & HF_LINK) {
		if (! unresolved) {
			fprintf(stderr,"Warning: contains unresolved references.\n");
		}
		unresolved++;
	}
	else if (outhead.oh_nrelo > 0 && !unresolved)
		fprintf(stderr, "Warning: relocation information present.\n");
	if ( outhead.oh_nsect!=LSECT && outhead.oh_nsect!=NSECT )
		fatal("Input file must have %d sections, not %ld\n",
			NSECT,outhead.oh_nsect) ;
	rd_sect(outsect, outhead.oh_nsect);
	/* A few checks */
	if ( outsect[BSSSG].os_flen != 0 )
		fatal("bss space contains initialized data\n") ;
	if ( !unresolved) {
		if (! follows(&outsect[BSSSG], &outsect[DATASG]))
			fatal("bss segment must follow data segment\n") ;
		if (! follows(&outsect[ROMSG],&outsect[TEXTSG]))
			fatal("rom segment must follow text\n") ;
		if (! follows(&outsect[DATASG], &outsect[ROMSG]))
			fatal("data segment must follow rom\n") ;
	}
	outsect[TEXTSG].os_size = outsect[ROMSG].os_base - outsect[TEXTSG].os_base;
	outsect[ROMSG].os_size = outsect[DATASG].os_base - outsect[ROMSG].os_base;
	outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
	if ( outsect[ROMSG].os_lign == 0x400 ) {
		/* 410/413 file with ROMSG in data space */
		rom_in_data = 1;
		magic= NMAGIC ;
		textsize= outsect[TEXTSG].os_size;
		datasize= outsect[ROMSG].os_size + outsect[DATASG].os_size ;
	} else
	if ( outsect[DATASG].os_lign == 0x400 ) {
		/* 410/413 file with ROMSG in instruction space */
		rom_in_data = 0;
		magic= NMAGIC ;
		textsize= outsect[TEXTSG].os_size + outsect[ROMSG].os_size ;
		datasize= outsect[DATASG].os_size ;
	} else {
		/* Plain 407 file */
		rom_in_data = 0;
		magic= OMAGIC ;
		textsize= outsect[TEXTSG].os_size + outsect[ROMSG].os_size ;
		datasize = outsect[DATASG].os_size;
	}
	bsssize = outsect[BSSSG].os_size;
	if (nflag && magic != NMAGIC) {
		fatal("illegal alignments.\n");
	}
	if (magic == NMAGIC && ! nflag) {
		if (datasize & 0x3ff) {
			int diff = 0x400 - (datasize & 0x3ff);

			if (bsssize >= diff) bsssize -= diff;
			else bsssize = 0;
			outsect[DATASG].os_size += diff;
			datasize += diff;
		}
		magic = ZMAGIC;
	}
	if ( outhead.oh_nsect==NSECT ) {
		if (! follows(&outsect[LSECT],&outsect[BSSSG]))
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}

	if (magic != OMAGIC && unresolved) {
		fatal("unresolved references with wrong magic number\n");
	}

	if (outsect[TEXTSG].os_base != ENTRY) {
		fatal("Illegal entry point.\n");
	}

	bh.magic = magic;
	bh.tsize = textsize;
	bh.bsize = bsssize;
	bh.dsize = datasize;
	bh.rtsize = 0;
	bh.rdsize = 0;
	bh.entry = ENTRY;

	/* Action at last */
	if (magic == ZMAGIC)  {
		lseek(output,(long) 0x400, 0);
	}
	else	lseek(output,(long) TOT_HDRSIZE,0);
	emits(&outsect[TEXTSG]) ;
	emits(&outsect[ROMSG]) ;
	emits(&outsect[DATASG]) ;
	if (unresolved) emit_relo();
	emit_symtab();
	bh.ssize = outhead.oh_nname * sizeof(struct sym);
	lseek(output,0L,0);
	cvlong(&(bh.magic));
	cvlong(&(bh.tsize));
	cvlong(&(bh.dsize));
	cvlong(&(bh.bsize));
	cvlong(&(bh.ssize));
	cvlong(&(bh.entry));
	cvlong(&(bh.rtsize));
	cvlong(&(bh.rdsize));
	writef(&bh, 1, (long) TOT_HDRSIZE);
	if ( outputfile_created  && !unresolved ) chmod(argv[2],0755);
	exit(0);
}

writef(addr,sz,cnt)
	char *addr;
	long cnt;
{
	cnt *= sz;

	while (cnt) {
		int i = cnt >= 0x4000 ? 0x4000 : cnt;

		cnt -= i;
		if (write(output, addr, i) < i) {
			fatal("write error\n");
		}
		addr += i;
	}
}

/*
 * Transfer the emitted byted from one file to another.
 */
emits(section) struct outsect *section ; {
	char		*p;
	char		*calloc(), *malloc();
	long sz = section->os_flen;

	rd_outsect(section - outsect);
	while (sz) {
		unsigned int i = (sz >= 0x4000 ? 0x4000 : sz);
		if (!(p = malloc(i))) {
			fatal("No memory.\n");
		}
		rd_emit(p, i);
		if (write(output, p, i) < i) {
			fatal("write error.\n");
		}
		free(p);
		sz -= i;
	}

	sz = section->os_size - section->os_flen;
	if (sz) {
		if (!(p = calloc(0x4000, 1))) {
			fatal("No memory.\n");
		}
		while (sz) {
			unsigned int i = (sz >= 0x4000 ? 0x4000 : sz);
			if (write(output, p, i) < i) {
				fatal("write error.\n");
			}
			sz -= i;
		}
		free(p);
	}
}

struct outname *ACKnames;

emit_relo()
{
	struct outrelo *ACKrelo;
	struct machrelo *MACHtrelo,*MACHdrelo;
	register struct outrelo *ap;
	register struct machrelo *mtp, *mdp;
	unsigned int cnt = outhead.oh_nrelo;

	ACKrelo = (struct outrelo *) calloc(cnt, sizeof(struct outrelo));
	MACHtrelo = (struct machrelo *) calloc(cnt, sizeof(struct machrelo));
	MACHdrelo = (struct machrelo *) calloc(cnt, sizeof(struct machrelo));
	ACKnames = (struct outname *) calloc(outhead.oh_nname, sizeof(struct outname));
	if (!(ap = ACKrelo) || !(mtp = MACHtrelo) || !(mdp = MACHdrelo) ||
	    !ACKnames) {
		fatal("No memory.\n");
	}
	rd_relo(ACKrelo, cnt);
	rd_name(ACKnames, outhead.oh_nname);
	while (cnt-- != 0) {
		register struct machrelo *mp;

		if (ap->or_sect - S_MIN <= ROMSG) mp = mtp++;
		else mp = mdp++;
		setlength(mp->relodata,(ap->or_type&RELSZ) >> 1);
		setpcrel(mp->relodata,(ap->or_type&RELPC != 0));
		mp->address = ap->or_addr;
		if (ap->or_sect == ROMSG+S_MIN) {
			mp->address += outsect[TEXTSG].os_size;
		}
		if (ap->or_nami < outhead.oh_nname) {
			if (ACKnames[ap->or_nami].on_type & S_EXT) {
				setsymbolnum(mp->relodata, ap->or_nami);
				setextern(mp->relodata,1);
			}
			else {
				patch(ap, &ACKnames[ap->or_nami], mp);
			}
		}
		else {
			setsymbolnum(mp->relodata, N_ABS);
		}
		cvlong(&(mp->address));
		cvlong(&(mp->relodata));
		ap++;
	}
	bh.rtsize = (char *) mtp - (char *) MACHtrelo;
	bh.rdsize = (char *) mdp - (char *) MACHdrelo;
	writef(MACHtrelo, 1, bh.rtsize);
	writef(MACHdrelo, 1, bh.rdsize);
	free(ACKrelo);
	free(MACHtrelo);
	free(MACHdrelo);
}

long
get(sz)
{
	char buf[10];
	long l = 0;
	register char *p = buf + sz;

	read(output,buf,sz);
	while (sz--) {
		l = (l << 8) | (*--p & 0377);
	}
	return l;
}

put(l,sz)
	long l;
{
	char buf[10];
	register char *p = buf;

	*p++ = l;
	*p++ = l >> 8;
	*p++ = l >> 16;
	*p++ = l >> 24;
	p -= sz;
	if (write(output, p, sz) < sz) {
		fatal("write error.\n");
	}
}

patch(ap, an, mp)
	register struct outrelo *ap;
	register struct outname *an;
	register struct machrelo *mp;
{
	int whichsect = (an->on_type & S_TYP) - S_MIN;
	long correction = 0;
	long where = TOT_HDRSIZE+ap->or_addr;
	long X;
	long here;
	int sz;

	if (!(an->on_type & S_SCT)) {
		fprintf(stderr,"funny on_type %x\n", an->on_type);
	}
	switch(whichsect) {
	case TEXTSG:
		setsymbolnum(mp->relodata,N_TEXT);
		return;
	case DATASG:
		correction = outsect[ROMSG].os_size + outsect[TEXTSG].os_size;
		setsymbolnum(mp->relodata,N_DATA);
		break;
	case ROMSG:
		correction = outsect[TEXTSG].os_size;
		setsymbolnum(mp->relodata,N_TEXT);
		break;
	case BSSSG:
		correction = outsect[ROMSG].os_size + outsect[TEXTSG].os_size+
				outsect[DATASG].os_size;
		setsymbolnum(mp->relodata,N_BSS);
		break;
	default:
		assert(0);
	}

	switch(ap->or_sect - S_MIN) {
	case DATASG:
		where += outsect[ROMSG].os_size;
	case ROMSG:
		where += outsect[TEXTSG].os_size;
	case TEXTSG:
		break;
	default:
		assert(0);
	}
	here = lseek(output, 0L, 1);
	lseek(output, where, 0);
	sz = ap->or_type & RELSZ;
	X = get(sz) + correction;
	lseek(output, where, 0);
	put(X,sz);
	lseek(output, here, 0);
}

cvlong(l)
	long *l;
{
	long x = *l;
	char *p = (char *) l;

	*p++ = x;
	*p++ = x >> 8;
	*p++ = x >> 16;
	*p++ = x >> 24;
}

cvshort(s)
	short *s;
{
	int x = *s;
	char *p = (char *) s;

	*p++ = x;
	*p = x >> 8;
}

int
is_rest_local(A, i)
	register int i;
	register struct outname *A;
{
	while (i--) {
		if (A->on_type & S_EXT) return 0;
		A++;
	}
	return 1;
}

emit_symtab()
{
	register unsigned short i = outhead.oh_nname;
	register struct outname *A;
	struct sym *MACHnames;
	register struct sym *M;
	extern char *malloc(), *calloc();
	char *chars;
	long offX = OFF_CHAR(outhead) - 4;

	if (!(A = ACKnames)) {
	    	if (!(A = (struct outname *)
			calloc(i, sizeof(struct outname)))) {
			fatal("No memory.\n");
		}
		rd_name(A, outhead.oh_nname);
	}
	if (!(M = (struct sym *) calloc(i, sizeof(struct sym)))) {
		fatal("No memory.\n");
	}
	MACHnames = M;
	ACKnames = A;
	for (; i; i--, A++) {
		M->value = A->on_valu;
		M->desc = A->on_desc;
		if ((A->on_type & S_SCT) ||
		    (A->on_type & S_ETC) == S_FIL) {
			static int rest_local;
			if (! unresolved || rest_local || (rest_local = is_rest_local(A, i))) {
				outhead.oh_nname--;
				continue;
			}
		}
		if (A->on_type & S_STB) {
			M->type = A->on_type >> 8;
		}
		else if (A->on_type & S_COM) {
			M->type = N_UNDF | N_EXT;
		}
		else switch(A->on_type & S_TYP) {
			case S_UND:
				switch(A->on_type & S_ETC) {
				default:
					M->type = N_UNDF;
					break;
				case S_MOD:
					M->type = N_FN;
					break;
				case S_LIN:
					M->type = N_ABS;
					break;
				}
				break;
			case S_ABS:
				M->type = N_ABS;
				break;
			case S_MIN + TEXTSG:
				M->type = N_TEXT; 
				break;
			case S_MIN + ROMSG:
				if (unresolved) {
					M->value += outsect[TEXTSG].os_size;
				}
				M->type = (rom_in_data ? N_DATA : N_TEXT);
				break;
			case S_MIN + DATASG:
				if (unresolved) {
					M->value += outsect[TEXTSG].os_size +
						    outsect[ROMSG].os_size;
				}
				M->type = N_DATA;
				break;
			case S_MIN + BSSSG:
				if (unresolved) {
					M->value += outsect[TEXTSG].os_size +
						    outsect[ROMSG].os_size +
						    outsect[DATASG].os_size;
				}
				M->type = N_BSS;
				break;
			case S_MIN + LSECT:
				M->type = N_BSS;
				break;
			default:
				fprintf(stderr,"warning: unknown s_type: %d\n",
					A->on_type & S_TYP);
		}
		if (A->on_type & S_EXT) M->type |= N_EXT;
		M->name = A->on_foff;
		M++;
	}
	M = MACHnames;
	for (i = outhead.oh_nname; i; i--, M++) {
		if (M->name) {
			M->name -= offX;
		}
		else M->name = outhead.oh_nchar + 3;	/* pointer to nullbyte */
		cvlong(&(M->name));
		cvlong(&(M->value));
		cvshort(&(M->desc));
	}
	writef(MACHnames, sizeof(struct sym), (long) outhead.oh_nname);
	free(MACHnames);
	free(ACKnames);
	if ((unsigned) outhead.oh_nchar != outhead.oh_nchar ||
	    !( chars = malloc((unsigned) outhead.oh_nchar))) {
		fatal("No memory\n.");
	}
	put(outhead.oh_nchar+4,4);
	rd_string(chars,outhead.oh_nchar);
	writef(chars, 1, outhead.oh_nchar);
	free(chars);
}

/* VARARGS1 */
fatal(s, a1, a2)
	char	*s;
{
	fprintf(stderr,"%s: ",program) ;
	fprintf(stderr, s, a1, a2);
	if (outputfile_created)
		unlink(output_file);
	exit(-1);
}

rd_fatal() { fatal("read error.\n"); }
