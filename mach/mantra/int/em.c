/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include <stdio.h>
#include <em_path.h>
#include <local.h>

char rcs_id[]	=	"$Header$" ;

#define MAGIC 07255

struct header {
	short h_magic;		/* Magic number */
	short h_flags;		/* See below for defines */
	short h_unresolved;	/* Cannot run if nonzero */
	short h_version;	/* Check for VERSION */
	short h_wsize;
	short h_psize;
	short h_unused[2];	/* left over */
} header;

#define TEST	001
#define PROFILE	002
#define FLOW	004
#define COUNT	010
char em_dir[64] = EM_DIR;	/* easier to patch */
char *defargv[] = {
	"em",
	"e.out",
	0
};

char interpret[BUFSIZ];
char flags[5];
char tflg,fflg,cflg,pflg;

main(argc,argv) char **argv; {
	char *file;
	int fildes;

	while (argc>1 && (argv[1][0]=='-' || argv[1][0]=='+')) {
		switch(argv[1][1]) {
		case 't':	tflg=argv[1][0]; break;
		case 'c':	cflg=argv[1][0]; break;
		case 'f':	fflg=argv[1][0]; break;
		case 'p':	pflg=argv[1][0]; break;
		default:
			fprintf(stderr,"Bad flag %c\n",argv[1][1]);
			exit(-1);
		}
		argv[1] = argv[0];
		argc--;argv++;
	}
	if (argc==1)
		argv= defargv;
	file=argv[1];
	if ((fildes=open(file,0))<0) {
		perror(file);
		exit(8) ;
	}
	header.h_magic= r2b(fildes,file) ;
	header.h_flags= r2b(fildes,file) ;
	header.h_unresolved= r2b(fildes,file) ;
	header.h_version= r2b(fildes,file) ;
	header.h_wsize= r2b(fildes,file) ;
	header.h_psize= r2b(fildes,file) ;
	header.h_unused[0]= r2b(fildes,file) ;
	header.h_unused[1]= r2b(fildes,file) ;
	if (header.h_magic != MAGIC) {
		fprintf(stderr,"%s not in correct format\n",file);
		exit(-1);
	}
	if (header.h_version != VERSION) {
		fprintf(stderr,"%s obsolete, recompile\n",file);
		exit(-1);
	}
	if (header.h_unresolved != 0) {
		fprintf(stderr,
			"%s has unresolved references, cannot run it\n",file);
		exit(-1);
	}
	if (tflg)
		flags[0] = tflg=='+' ? 't' : '-';
	else
		flags[0]= header.h_flags&TEST ? 't' : '-';
	if (fflg)
		flags[1] = fflg=='+' ? 'f' : '-';
	else
		flags[1]= header.h_flags&FLOW ? 'f' : '-';
	if (cflg)
		flags[2] = cflg=='+' ? 'c' : '-';
	else
		flags[2]= header.h_flags&COUNT ? 'c' : '-';
	if (pflg)
		flags[3] = pflg=='+' ? 'p' : '-';
	else
		flags[3]= header.h_flags&PROFILE ? 'p' : '-';
	sprintf(interpret,"%s/lib.bin/em%d%d/em_%s",
		em_dir,header.h_wsize,header.h_psize,flags);
	execv(interpret,argv);
	fprintf(stderr,"Interpreter %s not available\n",interpret);
}

r2b(fildes,file) char *file ; {
	char rd2[2] ;
	if ( read(fildes,rd2,sizeof rd2)!=sizeof rd2) {
		fprintf(stderr,"%s too short\n",file);
		exit(-1);
	}
	return (rd2[0]&0xFF) | ( (rd2[1]&0xFF)<<8 ) ;
}
