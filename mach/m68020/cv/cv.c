/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* This is a special cv.c program for testing purposes ;
 * it converts to a stripped version of COFF
 */

#include <stdio.h>

typedef struct filehdr {
	unsigned short	f_magic;	/* magic number */
	unsigned short	f_nscns;	/* number of sections */
	long		f_timdat;	/* time & date stamp */
	long		f_symptr;	/* file pointer to symtab */
	long		f_nsyms;	/* number of symtab entries */
	unsigned short	f_opthdr;	/* sizeof(optional hdr) */
	unsigned short	f_flags;	/* flags */
} FILHDR;

typedef	struct aouthdr {
	short	magic;
	short	vstamp; 
	long	tsize;
	long	dsize;
	long	bsize;
	long	entry;
	long	text_start;
	long	data_start;
} AOUTHDR;

typedef struct scnhdr {
	char		s_name[8];
	long		s_paddr;
	long		s_vaddr;
	long		s_size;
	long		s_scnptr;
	long		s_relptr;
	long		s_lnnoptr;
	unsigned short	s_nreloc;
	unsigned short	s_nlnno;
	long		s_flags;
} SCNHDR;

#define AOUTHSZ sizeof(AOUTHDR)
#define	SCNHSZ	sizeof(SCNHDR)
#define	FILHSZ	sizeof(FILHDR)

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

#define readf(a, b, c)	fread((a), (b), (int)(c), input)
#define writef(a, b, c)	fwrite((a), (b), (int)(c), output)

/* Output file definitions and such */

#define TS 0
#define DS 1
#define BS 2
#define NS 3

FILHDR	filh;
AOUTHDR	aouth;
SCNHDR	scnh[NS];

#define ENTRY 0
#define MC68MAGIC 0520
#define MYFLAGS 01017

#define STYP_TEXT 0x20
#define STYP_DATA 0x40
#define STYP_BSS  0x80

#define _TEXT ".text"
#define _DATA ".data"
#define _BSS  ".bss"

#define TEXTSG	0
#define ROMSG	1
#define DATASG	2
#define BSSSG	3
#define LSECT	BSSSG+1
#define NSECT	LSECT+1


FILE		*input;
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
	long			textsize, datasize, bsssize;
	extern long		ftell();
	long			filepos;
	int			scn;

	ASSERT(sizeof(struct outhead) == SZ_HEAD);
	ASSERT(sizeof(struct outsect) == SZ_SECT);

	input = stdin; output = stdout;
	program= argv[0] ;
	if ( argc>1 && argv[1][0]=='-' ) {
		flag=argv[1][1] ;
		argc-- ; argv++ ;
	}
	switch (argc) {
	case 1:	break;
	case 3:	if ((output = fopen(argv[2], "w")) == (FILE *)0)
			fatal("Can't write %s.\n", argv[2]);
		output_file = argv[2];
		outputfile_created = 1;
		/* FALLTHROUGH */
	case 2:	if ((input = fopen(argv[1], "r")) == (FILE *)0)
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s <as object> <dl object>.\n", argv[0]);
	}
	if ( !rhead(input,&outhead) )
		fatal("Reading header failed.\n");
	if (BADMAGIC(outhead))
		fatal("Not an ack object file.\n");
	if (outhead.oh_nrelo > 0)
		fprintf(stderr, "Warning: relocation information present.\n");
	if ( outhead.oh_nsect!=LSECT && outhead.oh_nsect!=NSECT )
		fatal("Input file must have %d sections, not %ld\n",
			NSECT,outhead.oh_nsect) ;
	for ( nsect=0 ; nsect<outhead.oh_nsect ; nsect++ )
		if ( !rsect(input,&outsect[nsect]) )
			fatal("Reading section table failed.\n");
	/* A few checks */
	if ( outsect[TEXTSG].os_base != ENTRY)
		fatal("text must start at %lx not at 0x%lx\n", ENTRY,
			outsect[TEXTSG].os_base) ;
	if ( outsect[BSSSG].os_flen != 0 )
		printf("Warning: bss space contains initialized data\n") ;
			/* as actually writes zeroes in the bss segment */
	if ( ! follows(&outsect[BSSSG], &outsect[DATASG]))
		fatal("bss segment must follow data segment\n") ;

	/* 410 file with ROMSG in instruction space */
	rom_in_data = 0;
	magic= 0410 ;
	textsize= (outsect[ROMSG].os_base - outsect[TEXTSG].os_base) 
			+ outsect[ROMSG].os_size ;
	datasize= outsect[BSSSG].os_base - outsect[DATASG].os_base ;
	if ( ! follows(&outsect[ROMSG], &outsect[TEXTSG]))
		fatal("rom segment must follow text\n") ;

	outsect[TEXTSG].os_size = outsect[ROMSG].os_base - outsect[TEXTSG].os_base;
	outsect[DATASG].os_size = datasize;
	bsssize = outsect[BSSSG].os_size;
	if ( outhead.oh_nsect==NSECT ) {
		if ( ! follows(&outsect[LSECT], &outsect[BSSSG]))
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}

	filh.f_magic	= MC68MAGIC;
	filh.f_nscns	= 3;	/* three sections: .text, data, .bss */
	filh.f_timdat	= (long) time(0); /* doesn't really matter */
	filh.f_symptr	= 0L;	/* no symbol table */
	filh.f_nsyms	= 0L;	/* ditto */
	filh.f_opthdr	= AOUTHSZ;
	filh.f_flags	= MYFLAGS;

	aouth.magic	= magic;
	aouth.vstamp	= 0;
	aouth.tsize	= textsize;
	aouth.dsize	= datasize;
	aouth.bsize	= bsssize;
	aouth.entry	= ENTRY;
	aouth.text_start= ENTRY;
	aouth.data_start= outsect[DATASG].os_base;

	strcpy(scnh[TS].s_name, _TEXT);
	strcpy(scnh[DS].s_name, _DATA);
	strcpy(scnh[BS].s_name, _BSS);

	scnh[TS].s_vaddr = (scnh[TS].s_paddr = aouth.text_start);
	scnh[DS].s_vaddr = (scnh[DS].s_paddr = aouth.data_start);
	scnh[BS].s_vaddr = (scnh[BS].s_paddr = aouth.data_start + datasize);

	scnh[TS].s_size = textsize;
	scnh[DS].s_size = datasize;
	scnh[BS].s_size = bsssize;

	scnh[TS].s_scnptr = FILHSZ + AOUTHSZ + 3 * SCNHSZ;
	scnh[DS].s_scnptr = FILHSZ + AOUTHSZ + 3 * SCNHSZ + textsize;
	scnh[BS].s_scnptr = 0L;	/* always 0 for .bss section */

	for (scn=TS; scn<=BS; scn++) {
		scnh[scn].s_relptr = 0L;	/* no relocation */
		scnh[scn].s_lnnoptr = 0L;	/* no line numbers */
		scnh[scn].s_nreloc = 0;		/* no relocation */
		scnh[scn].s_nlnno = 0;		/* no line numbers */
	}

	scnh[TS].s_flags = STYP_TEXT;
	scnh[DS].s_flags = STYP_DATA;
	scnh[BS].s_flags = STYP_BSS;

	/* now write the new file headers */
	ffwrite(&filh, FILHSZ, 1);
	ffwrite(&aouth, AOUTHSZ, 1);
	ffwrite(scnh, SCNHSZ, 3);	/* write all three section headers */

	emits(&outsect[TEXTSG]) ;
	emits(&outsect[ROMSG]) ;
	emits(&outsect[DATASG]) ;

	if ( outputfile_created ) chmod(argv[2],0755);
	return 0;
}

ffwrite(ptr, size, nitems)
char *ptr;
{
	writef(ptr, size, nitems);
	if (ferror(output))
		fatal("output write error\n");
}

/*
 * Transfer the emitted bytes from one file to another.
 */
emits(section) struct outsect *section ; {
	register long	n ;
	register int	blk;
	char		buffer[BUFSIZ];

	n= section->os_flen ;
	while (n > 0) {
		blk = n > BUFSIZ ? BUFSIZ : n;
		readf(buffer, 1, blk);
		writef(buffer, 1, blk);
		n -= blk;
	}
	if ((n=section->os_size - section->os_flen) > 0) {
		fseek(output, n-1, 1);
		writef("\0",1,1);
	}
}


rhead(f,head) struct outhead *head ; FILE *f ; {
	char buf[SZ_HEAD] ;
	if ( fread(buf,SZ_HEAD,1,f)!=1 ) return 0 ;
	iconvert(buf,(char *)head,SF_HEAD) ;
	return 1 ;
}

rsect(f,sect) struct outsect *sect ; FILE *f ; {
	char buf[SZ_SECT] ;
	if ( fread(buf,SZ_SECT,1,f)!=1 ) return 0 ;
	iconvert(buf,(char *)sect,SF_SECT) ;
	return 1 ;
}


iconvert(buf,str,fmt) char *buf, *str, *fmt ; {
	register char *nf, *ni, *no ;
	int last, i ;
	long value ;
	ni=buf ; no=str ; nf=fmt ;
	while ( last = *nf++ ) {
		last -= '0' ;
		if ( last<1 || last >9 ) fatal("illegal out.h format string\n");
		value=0 ;
		i=last ;
		while ( i-- ) {
			value = (value<<8) + (ni[i]&0xFF) ;
		}
		switch ( last ) {
		case 0 : break ;
		case 1 : *no= value ; break ;
		case 2 : *(unsigned short *)no = value ; break ;
		case 4 : *(long *)no = value ; break ;
		default :
			 fatal("illegal out.h format string\n");
		}
		ni += last ; no += last ;
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
