/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/*
 * The assumption is that this program is run on the same machine the
 * assembler has run. So the assembler's idea of long fits cv's idea.
 * The long's produced in the output file have the m68000 format.
 * The idea's of the longs read in can be changed with the following
 * flags:
 *  -v		Vax longs
 *  -p		Pdp 11 longs
 *  -m		M68000 longs
 */

#include <stdio.h>

char * prog ;

char flag ;

#define HDR_LENGTH	40

char hdr[HDR_LENGTH] ;

main(argc,argv) char **argv; {
	long addr,maxaddr;
	short count;
	
	maxaddr=0;
	if ( argc>1 && argv[1][0]=='-' ) {
		flag=argv[1][1] ;
		argc-- ; argv++ ;
	}
	if (argc != 3) {
		fprintf(stderr,"Usage: %s [-[vpm]] VU-a.out pmds-a.out\n",argv[0]);
		exit(-1);
	}
	prog=argv[0] ;
	if (freopen(argv[1],"r",stdin)==NULL) {
		fprintf(stderr,"%s: ",prog) ;
		perror(argv[1]);
		exit(-1);
	}
	if (freopen(argv[2],"w",stdout)==NULL) {
		fprintf(stderr,"%s: ",prog) ;
		perror(argv[2]);
		exit(-1);
	}
	while ( getaddr(&addr) && getcnt(&count) ) {
		fseek(stdout,addr+HDR_LENGTH,0);
		while (count--) {
			putchar(getchar());
			if ( ferror(stdout) ) {
				fprintf(stderr,"%s: write error\n",prog) ;
				unlink(argv[2]) ; /* remove the output */
				exit(2) ;
			}
			addr++;
		}
		if (addr>maxaddr)
			maxaddr = addr;
	}
	sethdr(0,0410L) ;	/* Magic */
	sethdr(8,maxaddr) ;
	fseek(stdout,0L,0);
	fwrite(hdr,sizeof(hdr),1,stdout);
	if ( ferror(stdout) ) {
		fprintf(stderr,"%s: write error\n",prog) ;
		unlink(argv[2]) ; /* remove the output */
		exit(2) ;
	}
	chmod(argv[2],0755);
	return 0;
}

getaddr(p_addr) long *p_addr ; {
	char in[4] ;
	short out[4] ;
	int i ;

	switch ( fread(in,1,4,stdin) ) {
	case 0 : return 0 ;
	case 4 : break ;;
	default : fprintf(stderr,"%s: Illegal input format\n",prog) ;
		  return 0 ;
	}
	switch ( flag ) {
	case 0 :	*p_addr = *((long *)in) ;
			return 1 ;
	case 'v' :
			out[0]=in[0] ; out[1]=in[1] ;
			out[2]=in[2] ; out[3]=in[3] ;
			break ;
	case 'm' :	
			out[0]=in[3] ; out[1]=in[2] ;
			out[2]=in[1] ; out[3]=in[0] ;
			break ;
	case 'p' :
			out[0]=in[2] ; out[1]=in[3] ;
			out[2]=in[0] ; out[3]=in[1] ;
			break ;
	default :
			fprintf(stderr,"%s: Unknown conversion %c(%o)\n",
				prog,flag,flag) ;
			exit(-1) ;
	}
	for ( i=0 ; i<4 ; i++ ) out[i] &= 0xFF ;
	*p_addr = 256 * ( 256 * ( 256 * out[3] + out[2] ) + out[1] ) + out[0];
	return 1 ;
}

getcnt(p_cnt) unsigned *p_cnt ; {
	char in[2] ;
	short out[2] ;
	int i ;

	switch ( fread(in,1,2,stdin) ) {
	case 2 : break ;;
	default : fprintf(stderr,"%s: Illegal input format\n",prog) ;
		  return 0 ;
	}
	switch ( flag ) {
	case 0 :	*p_cnt = *((short *)in) ;
			return 1 ;
	case 'v' :
	case 'p' :
			out[0]=in[0] ; out[1]=in[1] ;
			break ;
	case 'm' :	
			out[0]=in[1] ; out[1]=in[0] ;
			break ;
	default :
			fprintf(stderr,"%s: Unknown conversion %c(%o)\n",
				prog,flag,flag) ;
			exit(-1) ;
	}
	for ( i=0 ; i<2 ; i++ ) out[i] &= 0xFF ;
	*p_cnt = 256 * out[1] + out[0] ;
	return 1 ;
}

sethdr(off,val) long val ; {
	hdr[off]	= val>>24 ;
	hdr[off+1]	= val>>16 ;
	hdr[off+2]	= val>>8 ;
	hdr[off+3]	= val ;
}
