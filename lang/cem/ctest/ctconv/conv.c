/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/* Author: E.G. Keizer */

char rcs_id[] = "$Id$" ;

main() {
	t1() ;
	t2() ;
	return 0 ;
}

t1() {
	char c ; int i ; long l ; unsigned u ;
#ifndef NOFLOAT
	float f ;
#endif

	/* test conversions */

	/* first some conversions on constants */

	printf("(int) '\\377' = %d\n",(int) '\377') ;
	printf("(long) -1 = %ld\n",(long) -1 ) ;
#ifndef NOFLOAT
	printf("(float) 12 = %f\n",(float) 12 ) ;
	printf("(int) 3.14 = %d\n",(int) 3.14 ) ;
#endif
	printf("(int) 32767L = %d\n",(int) 32767L ) ;
	printf("(int) -32768L = %d\n",(int) -32768L ) ;
	printf("(char) 128L = %d\n",(char) 128L) ;
	printf("(char) 0377 = %d\n",(char) 0377 ) ;
	printf("(char) -1 = %d\n",(char) -1 ) ;
	printf("(char) 10000 = %d\n",(char) 10000 ) ;

	/* conversions from characters */
	printf("From character\n") ;
	c = 127 ;
	i=c ;
	l=c ;
	u=c ;
#ifndef NOFLOAT
	f=c ;
#endif
	printf("\tchar %5d, int %6d, unsigned %6o, long %11ld\n",c,i,u,l) ;
#ifndef NOFLOAT
	printf("\t\t\t\t\tfloat %f\n",f) ;
#endif
	c = -1 ;
	i=c ;
	l=c ;
	u=c ;
#ifndef NOFLOAT
	f=c ;
#endif
	printf("\tchar %5d, int %6d, unsigned %6o, long %11ld\n",c,i,u,l) ;
#ifndef NOFLOAT
	printf("\t\t\t\t\tfloat %f\n",f) ;
#endif
	c = 0377 ;
	i=c ;
	l=c ;
	u=c ;
#ifndef NOFLOAT
	f=c ;
#endif
	printf("\tchar %5d, int %6d, unsigned %6o, long %11ld\n",c,i,u,l) ;
#ifndef NOFLOAT
	printf("\t\t\t\t\tfloat %f\n",f) ;
#endif

	/* from integer */
	printf("From integer\n") ;
	i= -64 ;
	c=i ;
	l=i ;
	u=i ;
#ifndef NOFLOAT
	f=i ;
#endif
	printf("\tchar %5d, int %6d, unsigned %6o, long %11ld\n",c,i,u,l) ;
#ifndef NOFLOAT
	printf("\t\t\t\t\tfloat %f\n",f) ;
#endif
	/* from long */
	printf("From long\n") ;
	l = -3 ;
	c = l ;
	i = l ;
	u = l ;
#ifndef NOFLOAT
	f = l ;
#endif
	printf("\tchar %5d, int %6d, unsigned %6o, long %11ld\n",c,i,u,l) ;
#ifndef NOFLOAT
	printf("\t\t\t\t\tfloat %f\n",f) ;
#endif

	printf("Casts from long\n");
	l = 75000;
	printf("\tchar %5d, int %d, unsigned short %6o, long %11ld\n",
		(char) l,(int) l,(unsigned short)l ,l) ;

#ifndef NOFLOAT
	printf("From float\n") ;
	f = 121.5 ;
	c = f ;
	i = f ;
	u = f ;
	l = f ;
	printf("\tchar %5d, int %6d, unsigned %6o, long %11ld, float %f\n",c,i,u,l,f) ;
	f = 1e-4 ;
	c = f ;
	i = f ;
	u = f ;
	l = f ;
	printf("\tchar %5d, int %6d, unsigned %6o, long %11ld, float %f\n",c,i,u,l,f) ;
	f = 3276.6e1 ;
	i = f ;
	u = f ;
	l = f ;
	printf("\tint %6d, unsigned %6o, long %11ld, float %f\n",i,u,l,f) ;
	f = 1223432e3 ;
	l = f ;
	printf("\tlong %11ld, float %f\n",l,f) ;
#endif

	/* some special cases */
	{
		int a[4] ;

		l = 3 ; a[3]= -17 ;
		printf("a[l] (l==%ld) %d\n",l,a[l]) ;
		printf("a[3l] %d\n",a[3l] ) ;

	}
	return 0 ;
}

t2()
{
	long l1 = 0x1f010L;
	long l2;

	l2 = (unsigned short) l1;
	printf("(unsigned short) 0x1f010L = 0x%lx\n", l2);
	l2 = (short) l1;
	printf("(short) 0x1f010L = 0x%lx\n", l2);
}
