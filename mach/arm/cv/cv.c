/*
 *  Conversion from ack a.out to Arthur output format.
 *  All this really does is to output the sections, text first, 
 *  with no header or symbol table information.
 *
 *  Mostly pinched from the code for the m68k2 cv.
 *  V1.0, 08-Mar-89 AJM
 */

#include <stdlib.h>
#include <stdio.h>
#include "out.h"
#include "arm.h"

#define ASSERT(x) switch (2) { case 0: case (x): ; }

/*
 * Header and section table of ack object file.
 */
struct outhead	outhead;
struct outsect	outsect[S_MAX];
char		*stringarea ;

char	*ofile;
int	ofile_created;

char *program ;

char flag ;

#define readf(a, b, c)	fread((a), (b), (int)(c), input)
#define writef(a, b, c)	fwrite((a), (b), (int)(c), output)

/* Output file definitions and such */

#define HDR_LENGTH	32

char hdr[HDR_LENGTH] ;

#define TEXT	0
#define ROM	1
#define DATA	2
#define BSS	3
#define LSECT	BSS+1
#define NSECT	LSECT+1


#define N_EXT	040
#define N_UNDEF	00
#define N_ABS	01
#define N_TEXT	02
#define N_DATA	03
#define N_BSS	04
#define N_FN	037

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
	long			textsize ;
	long			datasize ;
	long 			ss;

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
		ofile = argv[2];
		ofile_created = 1;
		/* FALLTHROUGH */
	case 2:	if ((input = fopen(argv[1], "r")) == (FILE *)0)
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s <ack object> <riscos object>.\n", argv[0]);
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

	if ( outsect[TEXT].os_base != 0x8000 )
		fatal("text must start at 0x8000 not at 0x%lx\n",
			outsect[TEXT].os_base) ;

	if ( outsect[BSS].os_flen != 0 )
		fatal("bss space contains initialized data\n") ;

	if ( ! follows(&outsect[BSS], &outsect[DATA]))
		fatal("bss segment must follow data segment\n") ;

	if ( ! follows(& outsect[ROM], &outsect[TEXT]))
		fatal("rom segment must follow text\n") ;

	if ( ! follows(& outsect[DATA], &outsect[ROM]))
		fatal("data segment must follow rom\n") ;

	if ( outhead.oh_nsect==NSECT ) {
		if (! follows(&outsect[LSECT], &outsect[BSS]))
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}

	/* Action at last */
	fseek(output,0L,0);		/* Start of output file */
	emits(&outsect[TEXT]) ;		/* Write out the text segment */
	emits(&outsect[ROM]) ;		/* Then the rom */
	emits(&outsect[DATA]) ;		/* Then the data */

	if ( ferror(output) ) {
		fatal("output write error\n") ;
	}
	if ( ofile_created ) chmod(argv[2],0755);

	ss = 0;
	printf(" text = %ld\n",outsect[TEXT].os_size);
	printf(" rom  = %ld\n",outsect[ROM].os_size);
	printf(" data = %ld\n",outsect[DATA].os_size);
	printf(" bss  = %ld\n",outsect[BSS].os_size);
	ss += outsect[TEXT].os_size;
	ss += outsect[ROM].os_size;
	ss += outsect[DATA].os_size;
	ss += outsect[BSS].os_size;
        printf("Total memory %ld bytes, plus heap and stack\n",ss);


	return 0;
}

/*
 * Transfer the emitted byted from one file to another.
 * and zero fill the uninitialized space
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
	if ( section->os_flen!=section->os_size ) {
		for ( n=BUFSIZ-1 ; n ; n-- ) buffer[n]=0 ;
		n= section->os_size - section->os_flen ;
		while (n > 0) {
			blk = n > BUFSIZ ? BUFSIZ : n;
			writef(buffer, 1, blk);
			n -= blk;
		}
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
	if (ofile_created)
		unlink(ofile);
	exit(-1);
}
