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

/* Author: E.G. Keizer */

char rcs_id[] = "$Header$" ;

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
	printf(" u = %06o\n",u) ;
	printf(" u>>0  %06o\n", u>>0 ) ;
	printf(" u>>1  %06o\n", u>>1 ) ;
	printf(" u>>2  %06o\n", u>>2 ) ;
	printf(" u>>3  %06o\n", u>>3 ) ;
	printf(" u>>4  %06o\n", u>>4 ) ;
	printf(" u>>5  %06o\n", u>>5 ) ;
	printf(" u>>6  %06o\n", u>>6 ) ;
	printf(" u>>7  %06o\n", u>>7 ) ;
	printf(" u>>8  %06o\n", u>>8 ) ;
	printf(" u>>9  %06o\n", u>>9 ) ;
	printf(" u>>10 %06o\n", u>>10 ) ;
	printf(" u>>11 %06o\n", u>>11 ) ;
	printf(" u>>12 %06o\n", u>>12 ) ;
	printf(" u>>13 %06o\n", u>>13 ) ;
	printf(" u>>14 %06o\n", u>>14 ) ;
	printf(" u>>15 %06o\n", u>>15 ) ;
	printf(" u>>16 %06o\n", u>>16 ) ;
	printf(" u<<0  %06o\n", u<<0 ) ;
	printf(" u<<1  %06o\n", u<<1 ) ;
	printf(" u<<2  %06o\n", u<<2 ) ;
	printf(" u<<3  %06o\n", u<<3 ) ;
	printf(" u<<4  %06o\n", u<<4 ) ;
	printf(" u<<5  %06o\n", u<<5 ) ;
	printf(" u<<6  %06o\n", u<<6 ) ;
	printf(" u<<7  %06o\n", u<<7 ) ;
	printf(" u<<8  %06o\n", u<<8 ) ;
	printf(" u<<9  %06o\n", u<<9 ) ;
	printf(" u<<10 %06o\n", u<<10 ) ;
	printf(" u<<11 %06o\n", u<<11 ) ;
	printf(" u<<12 %06o\n", u<<12 ) ;
	printf(" u<<13 %06o\n", u<<13 ) ;
	printf(" u<<14 %06o\n", u<<14 ) ;
	printf(" u<<15 %06o\n", u<<15 ) ;
	printf(" u<<16 %06o\n", u<<16 ) ;
}

lshift() {
	long ll ;

	printf("Long shifts by constants\n") ;
	ll = 400000L - 0532 ;
	printf(" ll = %011O\n",ll) ;
	printf(" ll>>0  %011O\n", ll>>0 ) ;
	printf(" ll>>1  %011O\n", ll>>1 ) ;
	printf(" ll>>2  %011O\n", ll>>2 ) ;
	printf(" ll>>3  %011O\n", ll>>3 ) ;
	printf(" ll>>4  %011O\n", ll>>4 ) ;
	printf(" ll>>5  %011O\n", ll>>5 ) ;
	printf(" ll>>6  %011O\n", ll>>6 ) ;
	printf(" ll>>7  %011O\n", ll>>7 ) ;
	printf(" ll>>8  %011O\n", ll>>8 ) ;
	printf(" ll>>9  %011O\n", ll>>9 ) ;
	printf(" ll>>10 %011O\n", ll>>10 ) ;
	printf(" ll>>11 %011O\n", ll>>11 ) ;
	printf(" ll>>12 %011O\n", ll>>12 ) ;
	printf(" ll>>13 %011O\n", ll>>13 ) ;
	printf(" ll>>14 %011O\n", ll>>14 ) ;
	printf(" ll>>15 %011O\n", ll>>15 ) ;
	printf(" ll>>16 %011O\n", ll>>16 ) ;
	printf(" ll>>17 %011O\n", ll>>17 ) ;
	printf(" ll>>18 %011O\n", ll>>18 ) ;
	printf(" ll>>19 %011O\n", ll>>19 ) ;
	printf(" ll>>20 %011O\n", ll>>20 ) ;
	printf(" ll>>21 %011O\n", ll>>21 ) ;
	printf(" ll>>22 %011O\n", ll>>22 ) ;
	printf(" ll>>23 %011O\n", ll>>23 ) ;
	printf(" ll>>24 %011O\n", ll>>24 ) ;
	printf(" ll>>25 %011O\n", ll>>25 ) ;
	printf(" ll>>26 %011O\n", ll>>26 ) ;
	printf(" ll>>27 %011O\n", ll>>27 ) ;
	printf(" ll>>28 %011O\n", ll>>28 ) ;
	printf(" ll>>29 %011O\n", ll>>29 ) ;
	printf(" ll>>30 %011O\n", ll>>30 ) ;
	printf(" ll>>31 %011O\n", ll>>31 ) ;
	ll = 1 ;
	printf(" ll<<0  %011O\n", ll<<0 ) ;
	printf(" ll<<1  %011O\n", ll<<1 ) ;
	printf(" ll<<2  %011O\n", ll<<2 ) ;
	printf(" ll<<3  %011O\n", ll<<3 ) ;
	printf(" ll<<4  %011O\n", ll<<4 ) ;
	printf(" ll<<5  %011O\n", ll<<5 ) ;
	printf(" ll<<6  %011O\n", ll<<6 ) ;
	printf(" ll<<7  %011O\n", ll<<7 ) ;
	printf(" ll<<8  %011O\n", ll<<8 ) ;
	printf(" ll<<9  %011O\n", ll<<9 ) ;
	printf(" ll<<10 %011O\n", ll<<10 ) ;
	printf(" ll<<11 %011O\n", ll<<11 ) ;
	printf(" ll<<12 %011O\n", ll<<12 ) ;
	printf(" ll<<13 %011O\n", ll<<13 ) ;
	printf(" ll<<14 %011O\n", ll<<14 ) ;
	printf(" ll<<15 %011O\n", ll<<15 ) ;
	printf(" ll<<16 %011O\n", ll<<16 ) ;
	printf(" ll<<17 %011O\n", ll<<17 ) ;
	printf(" ll<<18 %011O\n", ll<<18 ) ;
	printf(" ll<<19 %011O\n", ll<<19 ) ;
	printf(" ll<<20 %011O\n", ll<<20 ) ;
	printf(" ll<<21 %011O\n", ll<<21 ) ;
	printf(" ll<<22 %011O\n", ll<<22 ) ;
	printf(" ll<<23 %011O\n", ll<<23 ) ;
	printf(" ll<<24 %011O\n", ll<<24 ) ;
	printf(" ll<<25 %011O\n", ll<<25 ) ;
	printf(" ll<<26 %011O\n", ll<<26 ) ;
	printf(" ll<<27 %011O\n", ll<<27 ) ;
	printf(" ll<<28 %011O\n", ll<<28 ) ;
	printf(" ll<<29 %011O\n", ll<<29 ) ;
	printf(" ll<<30 %011O\n", ll<<30 ) ;
}
uadd() {
	unsigned u ;
	int i ;

	u = 32760 ;
	for ( i=0 ; i<=16 ; ++i ) {
		printf("%2d %06o\n",i,u+i) ;
	}
}
