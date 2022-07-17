/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Fortunately we sold our PMDS machine. Unfortunately, we also sold the
 * documentation that came with it, so I don't know the a.out format
 * of the machine. This program is written partly by guessing, and partly from
 * an older conversion program for the PMDS, which put the whole program
 * in data. The produced object file does not contain a namelist.
 */

#include <stdio.h>
#include <out.h>

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


#define ENTRY 0
#define HDRSIZE	40

char hdr[HDRSIZE];


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
	if (! follows(&outsect[BSSSG], &outsect[DATASG]))
		fatal("bss segment must follow data segment\n") ;
	if ( outsect[ROMSG].os_lign == 0x8000 ) {
		/* 410 file with ROMSG in data space */
		rom_in_data = 1;
		magic= 0410 ;
		if (! follows(&outsect[DATASG], &outsect[ROMSG]))
			fatal("data segment must follow rom\n") ;
		textsize= outsect[TEXTSG].os_size ;
		outsect[ROMSG].os_size = outsect[DATASG].os_base - outsect[ROMSG].os_base;
		outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
		datasize= outsect[ROMSG].os_size + outsect[DATASG].os_size ;
	} else
	if ( outsect[DATASG].os_lign == 0x8000 ) {
		/* 410 file with ROMSG in instruction space */
		rom_in_data = 0;
		magic= 0410 ;
		if (! follows(&outsect[ROMSG], &outsect[TEXTSG]))
			fatal("rom segment must follow text\n") ;
		outsect[TEXTSG].os_size = outsect[ROMSG].os_base - outsect[TEXTSG].os_base;
		textsize= outsect[TEXTSG].os_size + outsect[ROMSG].os_size ;
		outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
		datasize= outsect[DATASG].os_size ;
	} else {
		/* Plain 407 file */
		rom_in_data = 1;
		magic= 0407 ;
		if (! follows(&outsect[ROMSG], &outsect[TEXTSG]))
			fatal("rom segment must follow text\n") ;
		if (! follows(&outsect[DATASG], &outsect[ROMSG]))
			fatal("data segment must follow rom\n") ;
		outsect[TEXTSG].os_size = outsect[ROMSG].os_base - outsect[TEXTSG].os_base;
		outsect[ROMSG].os_size = outsect[DATASG].os_base - outsect[ROMSG].os_base;
		outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
		textsize= outsect[TEXTSG].os_size ;
		datasize= outsect[ROMSG].os_size + outsect[DATASG].os_size ;
	}
	bsssize = outsect[BSSSG].os_size;
	if ( outhead.oh_nsect==NSECT ) {
		if (! follows(&outsect[LSECT], &outsect[BSSSG]))
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}

	/* Action at last */
	fseek(output,(long) HDRSIZE,0);
	emits(&outsect[TEXTSG]) ;
	emits(&outsect[ROMSG]) ;
	emits(&outsect[DATASG]) ;
	fseek(output,0L,0);
	sethdr(0, (long) magic);
	sethdr(4, textsize);
	sethdr(8, datasize);
	sethdr(12, bsssize);
	writef(hdr, 1, 40);
	if ( ferror(output) ) {
		fatal("output write error\n") ;
	}
	if ( outputfile_created ) chmod(argv[2],0755);
	return 0;
}


sethdr(off,val) long val ; {
	hdr[off]	= val>>24 ;
	hdr[off+1]	= val>>16 ;
	hdr[off+2]	= val>>8 ;
	hdr[off+3]	= val ;
}

/*
 * Transfer the emitted byted from one file to another.
 */
emits(section) struct outsect *section ; {
	char		*p;
	char		*calloc();
	long length = section->os_size;

	rd_outsect(section - outsect);
	while (section->os_size > 0) {
		int i = section->os_size > 10*1024 ? 10*1024 : section->os_size;

		section->os_size -= i;
		if (!(p = calloc(i, 1))) {
			fatal("No memory.\n");
		}
		rd_emit(p, i >= section->os_flen ? section->os_flen:(long) i);
		section->os_flen -= i;
		writef(p, 1, i);
		free(p);
	}
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
