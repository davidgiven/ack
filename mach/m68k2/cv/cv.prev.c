/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>
#include "out.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

#define ASSERT(x) switch (2) { case 0: case (x): ; }

/*
 * Header and section table of new format object file.
 */
struct outhead	outhead;
struct outsect	outsect[S_MAX];

char	*output_file;
int	output_file_created;

char *program ;

main(argc, argv)
	int	argc;
	char	*argv[];
{
	register int		nsect;
	register struct outsect	*sectp;
	register FILE		*input;
	register FILE		*output;

	ASSERT(sizeof(struct outhead) == SZ_HEAD);
	ASSERT(sizeof(struct outsect) == SZ_SECT);

	input = stdin; output = stdout;
	program= argv[0] ;
	switch (argc) {
	case 1:	break;
	case 3:	if ((output = fopen(argv[2], "w")) == (FILE *)0)
			fatal("Can't write %s.\n", argv[2]);
		output_file = argv[2];
		output_file_created = 1;
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
	for ( nsect=0 ; nsect<outhead.oh_nsect ; nsect++ )
		if ( !rsect(input,&outsect[nsect]) )
			fatal("Reading section table failed.\n");
	nsect = outhead.oh_nsect;
	sectp = outsect;
	while (nsect--) {
		register long	flen;
#ifdef DO_BSS
		register long	zero;
#endif DO_BSS
		long	base;
		short	cnt;
		char	buffer[BUFSIZ];

		base = sectp->os_base;
		flen = sectp->os_flen;
#ifdef DO_BSS
		zero = sectp->os_size - flen;
#endif DO_BSS

		while (flen) {
			cnt = flen > BUFSIZ ? BUFSIZ : flen;
			if (fread((char *)buffer, 1, cnt, input) != cnt)
				fatal("Reading code bytes failed.\n");
			if (fwrite((char *)&base, 4, 1, output) != 1)
				fatal("Writing start address failed.\n");
			if (fwrite((char *)&cnt, 2, 1, output) != 1)
				fatal("Writing byte count failed.\n");
			if (fwrite((char *)buffer, 1, cnt, output) != cnt)
				fatal("Writing byte count failed.\n");
			base += cnt;
			flen -= cnt;
		}
#ifdef DO_BSS
		while (zero) {
			cnt = zero > BUFSIZ ? BUFSIZ : zero;
			if (fwrite((char *)&base, 4, 1, output) != 1)
				fatal("Writing start address failed.\n");
			if (fwrite((char *)&cnt, 2, 1, output) != 1)
				fatal("Writing byte count failed.\n");
			if (fseek(output, (long)cnt, 1) < (long)0)
				fatal("Fseek failed.\n");
			base += cnt;
			zero -= cnt;
		}
#endif DO_BSS
		sectp++;
	}
	exit(0);
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

/* VARARGS1 */
fatal(s, a1, a2)
	char	*s;
{
	fprintf(stderr,"%s: ",program) ;
	fprintf(stderr, s, a1, a2);
	if (output_file_created)
		unlink(output_file);
	exit(-1);
}
