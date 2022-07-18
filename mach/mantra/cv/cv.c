/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include <stdio.h>
#include <out.h>

#define	FMAGIC	0407
#define	NMAGIC	0410

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

struct sym {
	char	stype;
	char	sympad;
	long	svalue;
};

#define N_UNDF	0
#define	N_ABS	01
#define	N_TEXT	02
#define	N_DATA	03
#define	N_BSS	04
#define	N_EXT	040

#define ASSERT(x) switch (2) { case 0: case (x): ; }

/*
 * Header and section table of new format object file.
 */
struct outhead	outhead;
struct outsect	outsect[S_MAX];

char	*output_file;
int	outputfile_created;

int rom_in_data;

char *program ;

char flag ;

#define writef(a, b, c)	fwrite((a), (b), (int)(c), output)

/* Output file definitions and such */

struct bhdr bh;

#define ENTRY 0x80000
#define TOT_HDRSIZE	(sizeof(struct bhdr))



#define TEXTSG	0
#define ROMSG	1
#define DATASG	2
#define BSSSG	3
#define LSECT	BSSSG+1
#define NSECT	LSECT+1

FILE		*output;

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
	long			magic ;
	long			textsize ;
	long			datasize ;
	long			bsssize;
	long			symstart;
	extern long		ftell();

	output = stdout;
	program= argv[0] ;
	if ( argc>1 && argv[1][0]=='-' ) {
		flag=argv[1][1] ;
		argc-- ; argv++ ;
	}
	switch (argc) {
	case 3:	if ((output = fopen(argv[2], "wb")) == (FILE *)0)
			fatal("Can't write %s.\n", argv[2]);
		output_file = argv[2];
		outputfile_created = 1;
		if (! rd_open(argv[1]))
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s <ACK object> <Mantra object>.\n", argv[0]);
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
	/* A few checks */
	if ( outsect[TEXTSG].os_base != ENTRY)
		fatal("text must start at %d not at 0x%lx\n", ENTRY,
			outsect[TEXTSG].os_base) ;
	if ( outsect[BSSSG].os_flen != 0 )
		fatal("bss space contains initialized data\n") ;
	if ( ! follows(&outsect[BSSSG], &outsect[DATASG]))
		fatal("bss segment must follow data segment\n") ;
	if ( outsect[ROMSG].os_lign == 0x8000 ) {
		/* 410 file with ROMSG in data space */
		if ( ! follows(&outsect[DATASG], &outsect[ROMSG]))
			fatal("data segment must follow rom\n") ;
		rom_in_data = 1;
		magic= NMAGIC ;
		textsize= outsect[TEXTSG].os_size ;
		datasize= outsect[BSSSG].os_base - outsect[ROMSG].os_base ;
		outsect[ROMSG].os_size = outsect[DATASG].os_base - outsect[ROMSG].os_base;
		outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
	} else
	if ( outsect[DATASG].os_lign == 0x8000 ) {
		/* 410 file with ROMSG in instruction space */
		if ( ! follows(&outsect[ROMSG], &outsect[TEXTSG]))
			fatal("rom segment must follow text\n") ;
		rom_in_data = 0;
		magic= NMAGIC ;
		outsect[TEXTSG].os_size = outsect[ROMSG].os_base - outsect[TEXTSG].os_base;
		textsize= outsect[TEXTSG].os_size + outsect[ROMSG].os_size ;
		outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
		datasize= outsect[DATASG].os_size ;
	} else {
		/* Plain 407 file */
		if ( ! follows(&outsect[ROMSG], &outsect[TEXTSG]))
			fatal("rom segment must follow text\n") ;
		if ( ! follows(&outsect[DATASG], &outsect[ROMSG]))
			fatal("data segment must follow rom\n") ;
		rom_in_data = 1;
		magic= FMAGIC ;
		outsect[TEXTSG].os_size = outsect[ROMSG].os_base - outsect[TEXTSG].os_base;
		outsect[ROMSG].os_size = outsect[DATASG].os_base - outsect[ROMSG].os_base;
		outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
		textsize= outsect[TEXTSG].os_size ;
		datasize= outsect[ROMSG].os_size + outsect[DATASG].os_size ;
	}
	bsssize = outsect[BSSSG].os_size;
	if ( outhead.oh_nsect==NSECT ) {
		if ( ! follows(&outsect[LSECT], &outsect[BSSSG]))
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}

	bh.fmagic = magic;

	bh.tsize = textsize;
	bh.bsize = bsssize;
	bh.dsize = datasize;
	bh.entry = ENTRY;

	/* Action at last */
	fseek(output,(long) TOT_HDRSIZE,0);
	emits(&outsect[TEXTSG]) ;
	emits(&outsect[ROMSG]) ;
	emits(&outsect[DATASG]) ;
	symstart = ftell(output);
	emit_symtab();
	bh.ssize = ftell(output) - symstart;
	fseek(output,0L,0);
	write_header(&bh);
	if ( ferror(output) ) {
		fatal("output write error\n") ;
	}
	if ( outputfile_created ) chmod(argv[2],0755);
	return 0;
}

/*
 * Transfer the emitted byted from one file to another.
 */
emits(section) struct outsect *section ; {
	char		*p;
	char		*calloc();

	rd_outsect(section - outsect);
	if (!(p = calloc(section->os_size, 1))) {
		fatal("No memory.\n");
	}
	rd_emit(p, section->os_flen);
	writef(p, 1, section->os_size);
	free(p);
}

emit_symtab()
{
	struct outname *ACK_names;
	register unsigned short i;
	register struct outname *A;
	register char x;
	register char *p;
	extern char *malloc(), *calloc();
	long l;
	long off = OFF_CHAR(outhead);
	char *chars;

	if ((unsigned) outhead.oh_nchar != outhead.oh_nchar ||
	    !( chars = malloc((unsigned) outhead.oh_nchar))) {
		fatal("No memory\n.");
	}
	rd_string(chars,outhead.oh_nchar);
	if ((unsigned) outhead.oh_nname != outhead.oh_nname ||
	    !(A = (struct outname *)
			calloc(outhead.oh_nname, sizeof(struct outname)))) {
		fatal("No memory.\n");
	}
	ACK_names = A;
	rd_name(ACK_names, outhead.oh_nname);
	for (i = 0; i < outhead.oh_nname; i++, A++) {
		if ((A->on_type & S_TYP) >= S_MIN + LSECT ||
		    A->on_foff == 0) continue;
		switch(A->on_type & S_TYP) {
			case S_UND:
				x = N_UNDF;
				break;
			case S_ABS:
				x = N_ABS;
				break;
			case S_MIN + TEXTSG:
				x = N_TEXT; 
				break;
			case S_MIN + ROMSG:
				x = (rom_in_data ? N_DATA : N_TEXT);
				break;
			case S_MIN + DATASG:
				x = N_DATA;
				break;
			case S_MIN + BSSSG:
				x = N_BSS;
				break;
			default:
				fprintf(stderr,"warning: unknown s_type: %d\n",
					A->on_type & S_TYP);
		}
		if (A->on_type & S_EXT) x |= N_EXT;
		putc(x,output);
		putc(0,output);
		write_long(A->on_valu);
		l = A->on_foff - off;
		if (l < 0 || l >= outhead.oh_nchar) {
			fatal("bad on_off: %ld\n",l);
		}
		p = &chars[l];
		while (x = *p++) {
			putc(x,output);
		}
		putc('\0',output);
	}
}

write_long(l)
	register long l;
{
	/*	write long "l" in 68000 order
	*/

	putc((int) (l >> 24), output);
	putc((int) (l >> 16), output);
	putc((int) (l >> 8), output);
	putc((int) l, output);
}

write_header(h)
	register struct bhdr *h;
{
	write_long(h->fmagic);
	write_long(h->tsize);
	write_long(h->dsize);
	write_long(h->bsize);
	write_long(h->ssize);
	write_long(h->rtsize);
	write_long(h->rdsize);
	write_long(h->entry);
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
