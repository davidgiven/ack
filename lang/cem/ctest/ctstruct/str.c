/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/* Author: E.G. Keizer */
static char rcs_id[]=	"$Id$" ;

/* test for structure parameters, assignment and return */
# define ASIZE 26

struct w1 {
	int w1_i ;
} ;
struct w2 {
	int w2_i ;
	long w2_l ;
} ;
struct w3 {
	char w3_a[ASIZE] ;
	unsigned w3_u ;
} ;

struct w1 es1 ;
struct w1 es2[3] ;

main() {
	asst() ;
	part() ;
	callt() ;
	return 0 ;

}

asst() {
	/* test structure assignment */
	struct w1 st1, st2, *st3 ;
	struct w2 s2t1, s2t2, *s2t3 ;
	struct w3 s3t1, s3t2, *s3t3 ;


	register int i ;

	printf("w1\n") ;
	st1.w1_i = 506 ;
	st2 = st1 ;
	printf("\tst2.w1_i %d\n",st2.w1_i) ;
	st3 = &st1 ;
	printf("\t(*st3).w1_i %d\n",(*st3).w1_i) ;
	es1.w1_i = 711 ;
	st1 = st2 = es1 ;
	printf("\tst1.w1_i %d\n",st1.w1_i) ;
	printf("\tst2.w1_i %d\n",st2.w1_i) ;
	es2[2] = st1 ;
	printf("\tes2[2].w1_i %d\n",es2[2].w1_i) ;

	st1.w1_i = -577 ;
	es1.w1_i = 577 ;
	for ( i=0 ; i<2 ; i++ ) {
		st2 = ( i ? st1 : es1 ) ;
		printf("\tst2.w1_i %d\n",st2.w1_i) ;
	}

	st1 = ( i , es1 ) ;
	printf("\tst1.w1_i %d\n",st1.w1_i) ;

	printf("w2\n") ;
	s2t1.w2_i = 18000 ;
	s2t1.w2_l = 31415 ;
	s2t2 = s2t1 ;
	printf("\ts2t2: .w2_i %d .w2_l %ld\n",s2t2.w2_i,s2t2.w2_l) ;
	s2t3 = &s2t2 ;
	printf("\ts2t3->w2_l %ld\n",s2t3->w2_l) ;

	printf("w3\n") ;
	for ( i = 0 ; i<ASIZE ; i++ ) {
		s3t1.w3_a[i]= 'a'+i ;
	}
	s3t1.w3_u = 0x8000 ;
	s3t2 = s3t1 ;
	s3t3 = &s3t1 ;
	for ( i = 0 ; i<ASIZE ; i++ ) {
		printf("s3t2.w3_a[%2d] %c\n",i,s3t2.w3_a[i]) ;
	}
	printf("s3t2.w3_u %x\n",s3t2.w3_u) ;
	s3t2.w3_u = 1415 ;
	for ( i = 0 ; i<ASIZE ; i++ ) {
		s3t2.w3_a[i]= 'A'+i ;
	}
	*s3t3 = s3t2 ;
	for ( i = 0 ; i<ASIZE ; i++ ) {
		printf("s3t1.w3_a[%2d] %c\n",i,s3t1.w3_a[i]) ;
	}
	printf("s3t1.w3_u %x",s3t1.w3_u) ;
}

struct w3 epars ;

part() {
	/* test structure parameters */

	struct w3 pars ;

	register i ;

	for ( i=0 ; i<ASIZE ; i++ ) {
		pars.w3_a[i]=i+1 ;
	}
	pars.w3_u = 281 ;
	printf("\nstructure parameters\n") ;
	psc(-1,pars,1000) ;
}

psc(before,str,after) int before, after ; struct w3 str ; {
	register int i ;

	printf("before %d\n",before) ;
	for ( i=0 ; i<ASIZE ; i++ ) {
		printf("str.w3_a[%2d]\t%d\n",i,str.w3_a[i]) ;
	}
	printf("str.w3_u %x\n",str.w3_u) ;
	printf("after %d\n",after) ;
}

callt() {
	/* test structure valued functions */
	extern struct w3 setp1(), setp2() ;
	struct w3 myp ;
	register int i ;

	printf("\nStucture valued functions\n") ;
	myp = setp1(ASIZE) ;
	printf("myp.w3_a:\n") ;
	for ( i=0 ; i<ASIZE ; i++ ) {
		printf("\t%2d\t%d\n",i,myp.w3_a[i]) ;
	}


	myp = setp2() ;
	for ( i=0 ; i<ASIZE ; i++ ) {
		printf("\t%2d\t%d\n",i,myp.w3_a[i]) ;
	}
}

struct w3 setp1(count) {
	struct w3 myp ;

	if ( count<=0 ) {
		return(myp) ;
	}
	myp = setp1(count-1) ;
	myp.w3_a[count-1] = 99-count-1 ;
	return(myp) ;
}

static struct w3 myp2 ;

struct w3 setp2() {
	struct w3 *w3p ;
	register i ;

	for ( i=0 ; i<ASIZE ; i++ ) {
		myp2.w3_a[i]= 99+i ;
	}
	w3p = &myp2 ;
	return(*w3p) ;
}
