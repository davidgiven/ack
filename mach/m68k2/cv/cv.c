/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include <stdio.h>
#include "out.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

/* BUG:
   There is a problem with Bleasdale 'nm' and the namelist produced
   by this program.
   nm always acts as if there were no name list, while adb
   understands a.out's with namelists without any complaint.
   The cause is unclear. All sizes in the header and file are correct.
   Setting the symbol table size to a size smaller than actually
   present in the a.out causes 'adb' to produce error messages
   about name list problems, but nm suddenly prints the namelist,
   INCLUDING the entrie outside the size indicated in the a.out header.
*/
#define ASSERT(x) switch (2) { case 0: case (x): ; }

/*
 * Header and section table of new format object file.
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
	if ( outsect[TEXT].os_base != 0x20000 )
		fatal("text must start at 0x20000 not at 0x%lx\n",
			outsect[TEXT].os_base) ;
	if ( outsect[BSS].os_flen != 0 )
		fatal("bss space contains initialized data\n") ;
	if ( ! follows(&outsect[BSS], &outsect[DATA]))
		fatal("bss segment must follow data segment\n") ;
	if ( outsect[ROM].os_lign == 0x8000 ) {
		/* 410 file with ROM in data space */
		if ( ! follows(&outsect[DATA], &outsect[ROM]))
			fatal("data segment must follow rom\n") ;
		magic= 0410 ;
		textsize= outsect[TEXT].os_size ;
		datasize= outsect[BSS].os_base - outsect[ROM].os_base ;
		outsect[ROM].os_size = outsect[DATA].os_base - outsect[ROM].os_base;
		outsect[DATA].os_size = outsect[BSS].os_base - outsect[DATA].os_base;
	} else
	if ( outsect[DATA].os_lign == 0x8000 ) {
		/* 410 file with ROM in instruction space */
		if ( ! follows(& outsect[ROM], &outsect[TEXT].os_base))
			fatal("rom segment must follow text\n") ;
		magic= 0410 ;
		outsect[TEXT].os_size = outsect[ROM].os_base - outsect[TEXT].os_base;
		textsize= outsect[TEXT].os_size + outsect[ROM].os_size ;
		datasize= outsect[BSS].os_base - outsect[DATA].os_base ;
		outsect[DATA].os_size = datasize;
	} else {
		/* Plain 407 file */
		if ( ! follows(& outsect[ROM], &outsect[TEXT]))
			fatal("rom segment must follow text\n") ;
		if ( ! follows(& outsect[DATA], &outsect[ROM]))
			fatal("data segment must follow rom\n") ;
		magic= 0407 ;
		outsect[TEXT].os_size = textsize
			= outsect[ROM].os_base - outsect[TEXT].os_base;
		outsect[ROM].os_size = outsect[DATA].os_base - outsect[ROM].os_base;
		outsect[DATA].os_size = outsect[BSS].os_base - outsect[DATA].os_base;
		datasize= outsect[ROM].os_size + outsect[DATA].os_size ;
	}
	if ( outhead.oh_nsect==NSECT ) {
		if (! follows(&outsect[LSECT], &outsect[BSS]))
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}

	/* Action at last */
	fseek(output,(long)sizeof(hdr),0);
	emits(&outsect[TEXT]) ;
	emits(&outsect[ROM]) ;
	emits(&outsect[DATA]) ;
	names() ;
	set4(hdr,0,magic) ;
	set4(hdr,4,textsize) ;
	set4(hdr,8,datasize) ;
	set4(hdr,12,outsect[BSS].os_size) ;
	set4(hdr,28,0x20000L) ;	/* Entry point */
	fseek(output,0L,0);
	fwrite(hdr,sizeof(hdr),1,output);
	if ( ferror(output) ) {
		fatal("output write error\n") ;
	}
	if ( ofile_created ) chmod(argv[2],0755);
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

/*
 * Copy the name table, except the last outhead.oh_nsect.
 * These are assumed to be section names.
 *
 */
names()
{
	register unsigned	n = outhead.oh_nname - outhead.oh_nsect;
	int		type = 0 ;
	struct outname  outname ;
	char		buffer[100] ;
	char		*s1, *s2 ;
	long		length, fullength ;

	fseek(input,OFF_CHAR(outhead),0) ;
	chars() ;
	if ( !stringarea ) return ;
	fseek(input,OFF_NAME(outhead),0) ;
	fullength=0 ; buffer[1]=0 ;
	while (n--) {
		rname(input,&outname) ;
		if (outname.on_foff == 0)
			continue ;
		if (outname.on_type & S_ETC ) {
			switch (outname.on_type & S_ETC ) {
			/* ???
			case S_FIL :
				type |= N_FN ;
			*/
			case S_COM :
				break ;
			default :
				continue ;
			} 
		}
		set4(buffer,2,outname.on_valu ) ;
		switch (outname.on_type & S_TYP) {
		case S_UND:
			type = N_UNDEF;
			break;
		case S_ABS:
			type = N_ABS;
			break;
		case S_MIN + TEXT:
			type = N_TEXT;
			break;
		case S_MIN + DATA:
			type = N_DATA;
			break;
		case S_MIN + BSS:
		case S_MIN + LSECT:
			type = N_BSS;
			break ;
		}

		if (outname.on_type & S_EXT)
			type |= N_EXT;
		buffer[0]= type ;
		s1=buffer+6 ; s2= stringarea+outname.on_foff-OFF_CHAR(outhead);
		length= 6 ;
		while ( (++length<sizeof buffer) && (*s1++ = *s2++) ) ;
		buffer[sizeof buffer - 1]= 0 ; /* make sure of zero byte */
		writef(buffer, length, 1);
		fullength += length ;
	}
	/*
	 * The last outhead.oh_nsect names are discarded.
	 */
	fseek(input, (long)(SZ_NAME * outhead.oh_nsect), 1);
	set4(hdr,16,fullength) ;
}

/*
 * Grab the string area preceded by the long that indicates its size.
 *
 */
chars()
{
	register long	n = outhead.oh_nchar;
	register int	blk;
	char	 	*curr ;
	extern char	*malloc() ;

	if ( n==0 ) return ;
	if ( (unsigned)n != n ) fatal("string table too large\n") ;
	stringarea= malloc((unsigned)n) ;
	if ( !stringarea ) fatal("string table too large\n") ;

	curr= stringarea ;
	while (n > 0) {
		blk = n > BUFSIZ ? BUFSIZ : n;
		readf(curr, 1, blk);
		curr += blk ;
		n -= blk;
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

rrelo(f,relo) struct outrelo *relo ; FILE *f ; {
	char buf[SZ_RELO] ;
	if ( fread(buf,SZ_RELO,1,f)!=1 ) return 0 ;
	iconvert(buf,(char *)relo,SF_RELO) ;
	return 1 ;
}

rname(f,name) struct outname *name ; FILE *f ; {
	char buf[SZ_NAME] ;
	if ( fread(buf,SZ_NAME,1,f)!=1 ) return 0 ;
	iconvert(buf,(char *)name,SF_NAME) ;
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

set2(buf,off,val) char *buf ; int val ; {
	buf[off+0]	= val>>8 ;
	buf[off+1]	= val ;
}

set4(buf,off,val) char *buf ; long val ; {
	buf[off]	= val>>24 ;
	buf[off+1]	= val>>16 ;
	buf[off+2]	= val>>8 ;
	buf[off+3]	= val ;
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
