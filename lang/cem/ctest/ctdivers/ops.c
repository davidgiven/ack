/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/* Author: E.G. Keizer */

char rcs_id[] = "$Id$" ;

main() {

	assnull() ;
	ushift() ;
	lshift() ;
	uadd() ;
	return 0 ;
}

int a,b ;
assnull() {
	int c,d ;
	/* test a few cases handled especially by the cem-compiler */

	a= -1 ; b= -1 ; c= -1 ; d = -1 ;

	a=b=0 ;
	c=d=0 ;
	printf("a %d, b %d, c %d, d %d\n",a,b,c,d) ;
	a = b = c = d = -32 ;
	printf (" (a=0) %d, (c=0) %d\n",(a=0),(c=0) ) ;
	printf("a %d, b %d, c %d, d %d\n",a,b,c,d) ;

}
ushift() {
	unsigned u ;

	printf("Unsigned shifts by constants\n") ;
	u = 0150715 ;
	printf(" u = %6o\n",u) ;
	printf(" u>>0  %6o\n", u>>0 ) ;
	printf(" u>>1  %6o\n", u>>1 ) ;
	printf(" u>>2  %6o\n", u>>2 ) ;
	printf(" u>>3  %6o\n", u>>3 ) ;
	printf(" u>>4  %6o\n", u>>4 ) ;
	printf(" u>>5  %6o\n", u>>5 ) ;
	printf(" u>>6  %6o\n", u>>6 ) ;
	printf(" u>>7  %6o\n", u>>7 ) ;
	printf(" u>>8  %6o\n", u>>8 ) ;
	printf(" u>>9  %6o\n", u>>9 ) ;
	printf(" u>>10 %6o\n", u>>10 ) ;
	printf(" u>>11 %6o\n", u>>11 ) ;
	printf(" u>>12 %6o\n", u>>12 ) ;
	printf(" u>>13 %6o\n", u>>13 ) ;
	printf(" u>>14 %6o\n", u>>14 ) ;
	printf(" u>>15 %6o\n", u>>15 ) ;
	if (sizeof(unsigned) > 2) printf(" u>>16 %6o\n", u>>16 ) ;
	printf(" u<<0  %6o\n", u<<0 ) ;
	printf(" u<<1  %6o\n", u<<1 ) ;
	printf(" u<<2  %6o\n", u<<2 ) ;
	printf(" u<<3  %6o\n", u<<3 ) ;
	printf(" u<<4  %6o\n", u<<4 ) ;
	printf(" u<<5  %6o\n", u<<5 ) ;
	printf(" u<<6  %6o\n", u<<6 ) ;
	printf(" u<<7  %6o\n", u<<7 ) ;
	printf(" u<<8  %6o\n", u<<8 ) ;
	printf(" u<<9  %6o\n", u<<9 ) ;
	printf(" u<<10 %6o\n", u<<10 ) ;
	printf(" u<<11 %6o\n", u<<11 ) ;
	printf(" u<<12 %6o\n", u<<12 ) ;
	printf(" u<<13 %6o\n", u<<13 ) ;
	printf(" u<<14 %6o\n", u<<14 ) ;
	printf(" u<<15 %6o\n", u<<15 ) ;
	if (sizeof(unsigned) > 2) printf(" u<<16 %6o\n", u<<16 ) ;
}

lshift() {
	long ll ;

	printf("Long shifts by constants\n") ;
	ll = 400000L - 0532 ;
	printf(" ll = %11lo\n",ll) ;
	printf(" ll>>0  %11lo\n", ll>>0 ) ;
	printf(" ll>>1  %11lo\n", ll>>1 ) ;
	printf(" ll>>2  %11lo\n", ll>>2 ) ;
	printf(" ll>>3  %11lo\n", ll>>3 ) ;
	printf(" ll>>4  %11lo\n", ll>>4 ) ;
	printf(" ll>>5  %11lo\n", ll>>5 ) ;
	printf(" ll>>6  %11lo\n", ll>>6 ) ;
	printf(" ll>>7  %11lo\n", ll>>7 ) ;
	printf(" ll>>8  %11lo\n", ll>>8 ) ;
	printf(" ll>>9  %11lo\n", ll>>9 ) ;
	printf(" ll>>10 %11lo\n", ll>>10 ) ;
	printf(" ll>>11 %11lo\n", ll>>11 ) ;
	printf(" ll>>12 %11lo\n", ll>>12 ) ;
	printf(" ll>>13 %11lo\n", ll>>13 ) ;
	printf(" ll>>14 %11lo\n", ll>>14 ) ;
	printf(" ll>>15 %11lo\n", ll>>15 ) ;
	printf(" ll>>16 %11lo\n", ll>>16 ) ;
	printf(" ll>>17 %11lo\n", ll>>17 ) ;
	printf(" ll>>18 %11lo\n", ll>>18 ) ;
	printf(" ll>>19 %11lo\n", ll>>19 ) ;
	printf(" ll>>20 %11lo\n", ll>>20 ) ;
	printf(" ll>>21 %11lo\n", ll>>21 ) ;
	printf(" ll>>22 %11lo\n", ll>>22 ) ;
	printf(" ll>>23 %11lo\n", ll>>23 ) ;
	printf(" ll>>24 %11lo\n", ll>>24 ) ;
	printf(" ll>>25 %11lo\n", ll>>25 ) ;
	printf(" ll>>26 %11lo\n", ll>>26 ) ;
	printf(" ll>>27 %11lo\n", ll>>27 ) ;
	printf(" ll>>28 %11lo\n", ll>>28 ) ;
	printf(" ll>>29 %11lo\n", ll>>29 ) ;
	printf(" ll>>30 %11lo\n", ll>>30 ) ;
	printf(" ll>>31 %11lo\n", ll>>31 ) ;
	ll = 1 ;
	printf(" ll<<0  %11lo\n", ll<<0 ) ;
	printf(" ll<<1  %11lo\n", ll<<1 ) ;
	printf(" ll<<2  %11lo\n", ll<<2 ) ;
	printf(" ll<<3  %11lo\n", ll<<3 ) ;
	printf(" ll<<4  %11lo\n", ll<<4 ) ;
	printf(" ll<<5  %11lo\n", ll<<5 ) ;
	printf(" ll<<6  %11lo\n", ll<<6 ) ;
	printf(" ll<<7  %11lo\n", ll<<7 ) ;
	printf(" ll<<8  %11lo\n", ll<<8 ) ;
	printf(" ll<<9  %11lo\n", ll<<9 ) ;
	printf(" ll<<10 %11lo\n", ll<<10 ) ;
	printf(" ll<<11 %11lo\n", ll<<11 ) ;
	printf(" ll<<12 %11lo\n", ll<<12 ) ;
	printf(" ll<<13 %11lo\n", ll<<13 ) ;
	printf(" ll<<14 %11lo\n", ll<<14 ) ;
	printf(" ll<<15 %11lo\n", ll<<15 ) ;
	printf(" ll<<16 %11lo\n", ll<<16 ) ;
	printf(" ll<<17 %11lo\n", ll<<17 ) ;
	printf(" ll<<18 %11lo\n", ll<<18 ) ;
	printf(" ll<<19 %11lo\n", ll<<19 ) ;
	printf(" ll<<20 %11lo\n", ll<<20 ) ;
	printf(" ll<<21 %11lo\n", ll<<21 ) ;
	printf(" ll<<22 %11lo\n", ll<<22 ) ;
	printf(" ll<<23 %11lo\n", ll<<23 ) ;
	printf(" ll<<24 %11lo\n", ll<<24 ) ;
	printf(" ll<<25 %11lo\n", ll<<25 ) ;
	printf(" ll<<26 %11lo\n", ll<<26 ) ;
	printf(" ll<<27 %11lo\n", ll<<27 ) ;
	printf(" ll<<28 %11lo\n", ll<<28 ) ;
	printf(" ll<<29 %11lo\n", ll<<29 ) ;
	printf(" ll<<30 %11lo\n", ll<<30 ) ;
}
uadd() {
	unsigned u ;
	int i ;

	u = 32760 ;
	for ( i=0 ; i<=16 ; ++i ) {
		printf("%2d %6o\n",i,u+i) ;
	}
}
