/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

char rcs_id[] = "$Id$" ;

/* C-compiler test 1 */
/* This program can be used to test C-compilers */

#ifndef NOFLOAT
# define EPSD 1e-6
# define EPSF 1e-6
#endif

/* global counters */

int t,		/* the value indicates the number of the test procedure */
    ect,	/* error counter */
    tct;	/* count the number of test procedures called */

/****************************************************************************/
/*
 * The following is tested:
 * INTEGER CONSTANTS in test1
 * GLOBAL INTEGER VARIABLES in test2
 * LOCAL INTEGER VARIABLES in test3
 * GLOBAL LONG VARIABLES in test4
 * LOCAL LONG VARIABLES in test5
 * REAL ARITHMETIC in test6
 * GLOBAL RECORDS in test7
 * LOCAL RECORDS in test8
 * GLOBAL ARRAYS in test9
 * LOCAL ARRAYS in test10
 * GLOBAL POINTERS in test11
 */
/***************************************************************************/
char alstr[1000] ;
char *alptr = alstr ;

char *alloc(size) {
	register char *retval ;

	retval=alptr ;
	alptr += size ;
	if ( alptr-alstr>sizeof alstr ) {
		printf("allocation overflow\n") ;
		exit(8) ;
	}
	return(retval) ;
}

#ifndef NOFLOAT
double fabs(a) double a ; { return( a<0 ? -a : a) ; }
#endif



/* global variables for the test procedures */

int i,j,k,l,m;

long li,lj,lk,ll,lm;

#ifndef NOFLOAT
float xf, yf, zf;

double xd, yd, zd;
#endif

struct tp2 {
    char c1;
    int i,j;
#ifndef NOFLOAT
    float aaa;
    double bbb;
#endif
} r1, r2;

int p, *p1, *p11, **p2, ***p3, ****p4, *****p5;

struct tp2 *pp1, *pp2, *pp3;
int a1[20];
#ifndef NOFLOAT
float a2[20];
double a3[20];
#endif

main()
{
    tct = 0;
    ect = 0;
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
#ifndef NOFLOAT
    test11();
#endif
    printf("End of test program, %d tests completed, %d errors detected\n",
	tct,ect);
	return 0 ;
}



e(n)
int n;
{
    ect++;
    printf("Error %d in test%d \n",n,t);
}



test1()		/* testing integer constants */
{
    t = 1;
    tct++;
    if (0) e(1);
    if (!1) e(2);
    if ( 1+2 != 3 ) e(3);
    if (-500 - 234 != -734) e(4);
    if (-32 + 155 != 123) e(5);
    if ( 2*3 != 6) e(6);
    if ( 3*4*5*6 != 360) e(7);
    if ( 654-3*2 != 648) e(8);
    if (5*5 + 5*5 != 50) e(9);
    if ( 1+1-1+1-1+1-1+1-1+1 != 2) e(10);
/**********************************************************************/
    if ( ((((((((((((((((((((0)))))))))))))))))))) ) e(11);
    if ( (((-2))) - ((((-3)))) * (3+((2))) != 13 ) e(12);
    if ( 1+1 != 2 ) e(13);
    if ( 3333 + 258 != 3591) e(14);
    if (3*4 != 12) e(15);
    if (111*111 != 12321) e(16);
    if (50 / 5 != 10) e(17);
    if (7498 / 75 != 99) e(18);
    if (456 - 345 != 111) e(19);
    if (1+(-2) != -1) e(20);
    if (-3 * -4 != 12) e(21);
    if (-2 / 2 != -1) e(22);
    if (-5 / 1 != -5) e(23);
    if (-4 - -5 != 1) e(24);
    if ( 03 + 02 != 05) e(25);
    if ( 03456 + 88 != 03606 ) e(26);
    if ( 0100 * 23 != 02700 ) e(27);
    if ( 045 / 020 != 2 ) e(28);
    if ( 0472 - 0377 != 073 ) e(29);
    if ( 'a' != 'a' ) e(30);
    if ( 'a' + 'c' != 'b' + 'b' ) e(31);
    if ( 'z' * 'z' != 14884 ) e(32);
    if ( -'z' / 01 != -'z' ) e(33);
    if ( 077777 >> 3 != 07777 ) e(34);
    if ( 077777 >> 15 ) e(35);
    if ( ( 0124 & 07765 ) != 0124 ) e(37);
    if ( ( 34 & 31 ) != 2 ) e(38);
    if (( 5 | 013 | 020 ) != 31 ) e(39);
    if ( ( -7 ^ 3 ) != -6 ) e(40);
    if ( (07373 ^ 4632 ) != 016343 ) e(41);
    if ( (1+2+3)*(2+3+4)*(3+5+5) / 2 != ((3*((5+3+2)*10)+51)*6)/6 ) e(42);
    if ( (1000*2+5*7+13)/8 != 2*2*2*2*4*4 ) e(43);
    if ( ( 1*2*3*4*5*6*7 / 5040 ) != 5040 / 7 / 6 / 5 / 4 / 3 / 2 / 1 ) e(44);
    if ( (-(-(-(-(-(-(1))))))) != 1) e(45);
    if ( -                 1 != -((((((((1)))))))) ) e(46);
    if ( -1-1-1-1-1-1 != -6+3-3 ) e(47);
    if ( 2<1 ) e(48);
    if ( 2==3 ) e(49);
    if ( 2 != 2 ) e(50);
    if ( 2>3 ) e(51);
    if ( 2+0 != 2 ) e(52);
    if ( 2-0 != 2 ) e(53);
    if ( 2*0 != 0 ) e(54);
    if ( 0/1 != 0 ) e(55);
    if ( 0*0 != 0 ) e(56);
    if (32767 > 32767) e(57);
    if ( -32768 > -32767 ) e(58);
    if ( 0456 < 0400 ) e(59);
    if ( 0456 != ( 0400 | 050 | 06 ) ) e(60);
    if ( 2*2 + (2<<2) != 12 ) e(61);
    if ( 0 || 0 ) e(62);
    if ( 1 && 0 ) e(63);
    if ( ( 123 ? 123*4 : 345 ) != 492 ) e(64);
    if ( ( 0 ? 345 : 280 ) != 280 ) e(65);
    if ( ( 4>>10 ) + 3 != 3 ) e(66);
    if ( ! ( 111 || 23 && 0 ) ) e(67);
    if ( !1 ) e(68);
    if ( !0 == 0 ) e(69);
    if ( !!!!!!!!!!0 ) e(70);
    if ( 2*2*2*2 != 4*4 ) e(71);
    if ( 0 && 0 && 0 && 0000 && 000000000000 ) e(72);
    if ( 1 & 1 & 1 && 1 & 1 && 1 & 0 ) e(73);
    if ( 01 + 01 + 01 != 1 + 1 + 1 ) e(74);
    if ( 07 + 1 != 010 ) e(75);
    if ( ('a' & 017777) != 'a' ) e(76);
    if ( ( 3 >> 0 ) != 3 ) e(77);
    if ( ( 3 << 0 ) != 3 ) e(78);
    if ( ((((((((((3)))))))))) << ((((((((((((2)))))))))))) != 12 ) e(79);
    if ( (((3 << 4)) >> 4) != 3 ) e(80);
    if ( (2+'a')*'b' != 2*'b' + 'a'*'b' ) e(81);
    if ( 'a' * 'a' < 0 ) e(82);
    if ( ('a'-'a'+'a'-'a'+('a'-'a')*'h') >> 3 ) e(82);
    if ( 'z' - 01 != 'y' + 00 ) e(83);
    if (  'a' ^ 'a' ) e(84);
    if ( 'h' ^ 'h' ^ 'a' ^ 'a' ) e(85);
    if ( 0567 ^ (0500 | 060 | 7 ) ) e(86);
    if ( 0 ^ 0 ^ 0 ^ 00 ) e(87);
    if ( ( !0 ) ^ (!0) ) e(88);
    if ( ( !!!40 ) ^ (!!!050) ) e(89);
    if ( ( 6^7 ) * 345 != 345 ) e(90);
    if ( !!!!!!!!!!!!! 'k' ) e(91);
    if ( !!!((('k'))) ) e(92);
    if ( -0 != 0 ) e(93) ;
}




test2()		/* testing global integer variables */
{
    t = 2;
    tct++;
    i = 1;
    j = 2;
    k = 3;
    l = 4;
    m = 10;
    if ( i + j  != k ) e(1);
    if ( i + k != l ) e(2);
    if ( j - k != -i ) e(3);
    if ( j*(j+k) != m ) e(4);
    if ( -m != -(k+k+l) ) e(5);
    if ( i/i != 1 ) e(6);
    if ( m*m / m != m ) e(7);
    if ( 10*m != 100 ) e(8);
    if ( m * (-10) != -100 ) e(9);
    if ( j/k != 0 ) e(10);
    if ( 100/k != 33 ) e(11);
    if ( i+j*k+l+m / j+50 / k != 32 ) e(12);
    if ( j*k*m / 6 != 10 ) e(13);
    if ( (k>4) || (k>=4) || (k==4) ) e(14);
    if ( (m<j) || (m<=j) || (m==j) ) e(15);
    if ( i+j-k ) e(16);
    if ( j<i ) e(17);
    if ( j != j ) e(18);
    if ( i>j ) e(19);
    if ( (i>j ? k : k*j)  != 6 ) e(20);
    if ( (i<j ? k : k*j)  != 3 ) e(21);
    if ( j<<i != l ) e(22);
    if ( j>> i != i ) e(25);
    if ( i++ != 1 ) e(26);
    if ( --i != 1 ) e(27);
    if ( i-- != 1 ) e(28);
    if ( (i+j) && (i<0) || (m-10) && (064) ) e(29);
    if ( (i+j) && !(i>=0) || (m-10) && !(0) ) e(30);
    i = 2;
    j = 4;
    k = 8;
    l = 16;
    m = 32;
    if ( i != 02 ) e(31);
    if ( j != 04 ) e(32);
    if ( k != 010 ) e(33);
    if ( l != 020 ) e(34);
    if ( i & j ) e(35);
    if ( i & j & k & l & m ) e(36);
    if ( ! ( i & j & k & l & m | i ) ) e(37);
    if ( ( i >> 0 ) != i ) e(38);
    if ( (( i/i ) << 1 ) != 02 ) e(39);
    if ( ( i | (j) | (k) | (l) | (m) ) != i+j+k+l+m ) e(40);
    if (!(i^j) ) e(41);
    if ( !(i^j^k^l^m) ) e(42);
    if ( ( --i << 1 ) != 2 ) e(43);
    if ( ( i << 01 ) != 2 ) e(44);
    if ( i%j != i ) e(45);
    if ( k%l != k ) e(46);
    if (( (m/l) << i >> i ) != 2 ) e(47);
    if ( (i = j = k = l = m) != m ) e(48);
    if ( ( i!=j ) || ( j!=k ) || !(l==m) ) e(49);
    if ( (i<j) || (j>i) || (k<m) ) e(50);
    if ( (i%j) || (j%i) || (k%l) || (m%l) ) e(51);
    if ( (i% (j<<0) + ((j<<3)>>3)%(i/i)) ) e(52);
    if ( ! ( i++ == j++ ) ) e(53);
    if ( i != j ) e(54);
    if ( i++ != (j++) ) e(55);
    i = 1;
    j = i + 1;
    if ( -i != -i ) e(56);
    if ( i != --j ) e(57);
    if ( (((((i))))) != -(-(-(-(i)))) ) e(59);
    if ( j != 1 ) e(60);
}




test3()		/* testing local integer variables */
{
    int a,b,c,d,f;

    t = 3;
    tct++;
    a = 1;
    b = 2;
    c = 3;
    d = 4;
    f = 10;
    if ( a + b  != c ) e(1);
    if ( a + c != d ) e(2);
    if ( b - c != -a ) e(3);
    if ( b*(b+c) != f ) e(4);
    if ( -f != -(c+c+d) ) e(5);
    if ( a/a != 1 ) e(6);
    if ( f*f / f != f ) e(7);
    if ( 10*f != 100 ) e(8);
    if ( f * (-10) != -100 ) e(9);
    if ( b/c != 0 ) e(10);
    if ( 100/c != 33 ) e(11);
    if ( a+b*c+d+f / b+50 / c != 32 ) e(12);
    if ( b*c*f / 6 != 10 ) e(13);
    if ( (c>4) || (c>=4) || (c==4) ) e(14);
    if ( (f<b) || (f<=b) || (f==b) ) e(15);
    if ( a+b-c ) e(16);
    if ( b<a ) e(17);
    if ( b != b ) e(18);
    if ( a>b ) e(19);
    if ( (a>b ? c : c*b)  != 6 ) e(20);
    if ( (a<b ? c : c*b)  != 3 ) e(21);
    if ( b<<a != d ) e(22);
    if ( b>> a != a ) e(25);
    if ( a++ != 1 ) e(26);
    if ( --a != 1 ) e(27);
    if ( a-- != 1 ) e(28);
    if ( (a+b) && (a<0) || (f-10) && (064) ) e(29);
    if ( (a+b) && !(a>=0) || (f-10) && !(0) ) e(30);
    a = 2;
    b = 4;
    c = 8;
    d = 16;
    f = 32;
    if ( a != 02 ) e(31);
    if ( b != 04 ) e(32);
    if ( c != 010 ) e(33);
    if ( d != 020 ) e(34);
    if ( a & b ) e(35);
    if ( a & b & c & d & f ) e(36);
    if ( ! ( a & b & c & d & f | a ) ) e(37);
    if ( ( a >> 0 ) != a ) e(38);
    if ( (( a/a ) << 1 ) != 02 ) e(39);
    if ( ( a | (b) | (c) | (d) | (f) ) != a+b+c+d+f ) e(40);
    if (!(a^b) ) e(41);
    if ( !(a^b^c^d^f) ) e(42);
    if ( ( --a << 1 ) != 2 ) e(43);
    if ( ( a << 01 ) != 2 ) e(44);
    if ( a%b != a ) e(45);
    if ( c%d != c ) e(46);
    if (( (f/d) << a >> a ) != 2 ) e(47);
    if ( (a = b = c = d = f) != f ) e(48);
    if ( ( a!=b ) || ( b!=c ) || !(d==f) ) e(49);
    if ( (a<b) || (b>a) || (c<f) ) e(50);
    if ( (a%b) || (b%a) || (c%d) || (f%d) ) e(51);
    if ( (a%(b<<0)  + ((b<<3)>>3)%(a/a)) ) e(52);
    if ( ! ( a++ == b++ ) ) e(53);
    if ( a != b ) e(54);
    if ( a++ != (b++) ) e(55);
    a = 1;
    b = a + 1;
    if ( -a != -a ) e(56);
    if ( a != --b ) e(57);
    if ( (((((a))))) != -(-(-(-(a)))) ) e(59);
    if ( b != 1 ) e(60);
}




test4()		/* testing global long variables */
{
    t = 4;
    tct++;
    li = 1;
    lj = 2;
    lk = 3;
    ll = 4;
    lm = 10;
    if ( li + lj  != lk ) e(1);
    if ( li + lk != ll ) e(2);
    if ( lj - lk != -li ) e(3);
    if ( lj*(lj+lk) != lm ) e(4);
    if ( -lm != -(lk+lk+ll) ) e(5);
    if ( li/li != 1 ) e(6);
    if ( lm*lm / lm != lm ) e(7);
    if ( 10*lm != 100 ) e(8);
    if ( lm * (-10) != -100 ) e(9);
    if ( lj/lk != 0 ) e(10);
    if ( 100/lk != 33 ) e(11);
    if ( li+lj*lk+ll+lm / lj+50 / lk != 32 ) e(12);
    if ( lj*lk*lm / 6 != 10 ) e(13);
    if ( (lk>4) || (lk>=4) || (lk==4) ) e(14);
    if ( (lm<lj) || (lm<=lj) || (lm==lj) ) e(15);
    if ( li+lj-lk ) e(16);
    if ( lj<li ) e(17);
    if ( lj != lj ) e(18);
    if ( li>lj ) e(19);
    if ( (li>lj ? lk : lk*lj)  != 6 ) e(20);
    if ( (li<lj ? lk : lk*lj)  != 3 ) e(21);
    if ( lj<<li != ll ) e(22);
    if ( lj>> li != li ) e(25);
    if ( li++ != 1 ) e(26);
    if ( --li != 1 ) e(27);
    if ( li-- != 1 ) e(28);
    if ( (li+lj) && (li<0) || (lm-10) && (064) ) e(29);
    if ( (li+lj) && !(li>=0) || (lm-10) && !(0) ) e(30);
    li = 2;
    lj = 4;
    lk = 8;
    ll = 16;
    lm = 32;
    if ( li != 02 ) e(31);
    if ( lj != 04 ) e(32);
    if ( lk != 010 ) e(33);
    if ( ll != 020 ) e(34);
    if ( li & lj ) e(35);
    if ( li & lj & lk & ll & lm ) e(36);
    if ( ! ( li & lj & lk & ll & lm | li ) ) e(37);
    if ( ( li >> 0 ) != li ) e(38);
    if ( (( li/li ) << 1 ) != 02 ) e(39);
    if ( ( li | (lj) | (lk) | (ll) | (lm) ) != li+lj+lk+ll+lm ) e(40);
    if (!(li^lj) ) e(41);
    if ( !(li^lj^lk^ll^lm) ) e(42);
    if ( ( --li << 1 ) != 2 ) e(43);
    if ( ( li << 01 ) != 2 ) e(44);
    if ( li%lj != li ) e(45);
    if ( lk%ll != lk ) e(46);
    if (( (lm/ll) << li >> li ) != 2 ) e(47);
    if ( (li = lj = lk = ll = lm) != lm ) e(48);
    if ( ( li!=lj ) || ( lj!=lk ) || !(ll==lm) ) e(49);
    if ( (li<lj) || (lj>li) || (lk<lm) ) e(50);
    if ( (li%lj) || (lj%li) || (lk%ll) || (lm%ll) ) e(51);
    if ( (li%(lj<<0)  + ((lj<<3)>>3)%(li/li)) ) e(52);
    if ( ! ( li++ == lj++ ) ) e(53);
    if ( li != lj ) e(54);
    if ( li++ != (lj++) ) e(55);
    li = 1;
    lj = li + 1;
    if ( -li != -li ) e(56);
    if ( li != --lj ) e(57);
    if ( (((((li))))) != -(-(-(-(li)))) ) e(59);
    if ( lj != 1 ) e(60);
    li = 40000;
    lj = 80000;
    lk = 800000L;
    ll = -800000L;
    lm = 1200000L;
    if ( lk != -ll ) e(61);
    if ( 10 * li != 400000L ) e(62);
    if ( 2 * li != lj ) e(63);
    if ( -(-(-(-(li)))) != li ) e(64);
    if ( 10 * lj != lk ) e(65);
    if ( lm + lm != 2 * lm ) e(66);
    if ( lm - lm ) e(67);
    if ( lk / lk != 1 ) e(68);
    if ( lk / lj != 10 ) e(69);
    if ( lm / li != 30 ) e(70);
    if ( li + lj != lm / 10 ) e(71);
    if ( li - 40000 - 1 != lk - 800001L ) e(72);
    if ( li + li + li + li +li + li != lj + lj + lj ) e(73);
    if ( li > lj ) e(74);
    if ( lj > lk ) e(75);
    if ( lm < ll ) e(76);
    if ( (lm<1000000L) || (((lk-lj-lj*10)>0)) ) e(77);
    if ( lm / 01 != lm ) e(78);
    if ( lm * 01 != lm ) e(79);
    if ( lm + 'a' != lm + 'b' -1 ) e(80);
    if ( (lm % 'a') % 'a' != lm % 'a' ) e(81);
    if ( lm % lm ) e(82);
    if ( lj % li ) e(83);
    if ( (lm<<1) != lm * 2 ) e(84);
    if ( ! ( ( --lm % li ) + 1 ) ) e(86);
    if ( ( lj >> 1 ) ^ li ) e(87);
    li = 1;
    if ( li != 1 ) e(89);
    li <<= 20;
    lj = 2;
    if ( (lj<<19) != li ) e(90);
    li = lj = lk= ll = lm = -345678L;
    if ( (li != lj) || (lj != lk) || (ll != lm) ) e(91);
    if ( (li != lj) || (lj != lk) || (lk != ll) || (ll != lm) ) e(92);
    if ( li != -345678L ) e(93);
    li = 1 | 2;
    li <<= 20;
    lj = li & li & li & li & li | li | li | li;
    if ( li != lj ) e(94);
    if ( ! ( li & lj ) ) e(95);
    if ( li ^ lj ) e(96);
    if ( ! (li | lj) ) e(97);
    if ( (li >> 20) != 3 ) e(98);
    li = 20000;
    li *= 2;
    if ( li < 0 ) e(99);
    if ( 1 * li != li ) e(100);
    lj = 20000;
    if ( (lj<<1) != li ) e(101);
    if ( (5*lj)/10 != lj/2 ) e(102);
    if ( 4*lj != 1*01*2*2*lj ) e(103);
    li = lj = 30000;
    if ( li != li * lj / 30000 ) e(104);
    if ( ++li != ++lj ) e(105);
    lk = 5;
    ll = 150000L;
    if ( lk * (li-1) != ll ) e(106);
}




test5()		/* testing local long variables */
{
    long la, lb, lc, ld, lf;

    t = 5;
    tct++;
    la = 1;
    lb = 2;
    lc = 3;
    ld = 4;
    lf = 10;
    if ( la + lb  != lc ) e(1);
    if ( la + lc != ld ) e(2);
    if ( lb - lc != -la ) e(3);
    if ( lb*(lb+lc) != lf ) e(4);
    if ( -lf != -(lc+lc+ld) ) e(5);
    if ( la/la != 1 ) e(6);
    if ( lf*lf / lf != lf ) e(7);
    if ( 10*lf != 100 ) e(8);
    if ( lf * (-10) != -100 ) e(9);
    if ( lb/lc != 0 ) e(10);
    if ( 100/lc != 33 ) e(11);
    if ( la+lb*lc+ld+lf / lb+50 / lc != 32 ) e(12);
    if ( lb*lc*lf / 6 != 10 ) e(13);
    if ( (lc>4) || (lc>=4) || (lc==4) ) e(14);
    if ( (lf<lb) || (lf<=lb) || (lf==lb) ) e(15);
    if ( la+lb-lc ) e(16);
    if ( lb<la ) e(17);
    if ( lb != lb ) e(18);
    if ( la>lb ) e(19);
    if ( (la>lb ? lc : lc*lb)  != 6 ) e(20);
    if ( (la<lb ? lc : lc*lb)  != 3 ) e(21);
    if ( lb<<la != ld ) e(22);
    if ( lb>> la != la ) e(25);
    if ( la++ != 1 ) e(26);
    if ( --la != 1 ) e(27);
    if ( la-- != 1 ) e(28);
    if ( (la+lb) && (la<0) || (lf-10) && (064) ) e(29);
    if ( (la+lb) && !(la>=0) || (lf-10) && !(0) ) e(30);
    la = 2;
    lb = 4;
    lc = 8;
    ld = 16;
    lf = 32;
    if ( la != 02 ) e(31);
    if ( lb != 04 ) e(32);
    if ( lc != 010 ) e(33);
    if ( ld != 020 ) e(34);
    if ( la & lb ) e(35);
    if ( la & lb & lc & ld & lf ) e(36);
    if ( ! ( la & lb & lc & ld & lf | la ) ) e(37);
    if ( ( la >> 0 ) != la ) e(38);
    if ( (( la/la ) << 1 ) != 02 ) e(39);
    if ( ( la | (lb) | (lc) | (ld) | (lf) ) != la+lb+lc+ld+lf ) e(40);
    if (!(la^lb) ) e(41);
    if ( !(la^lb^lc^ld^lf) ) e(42);
    if ( ( --la << 1 ) != 2 ) e(43);
    if ( ( la << 01 ) != 2 ) e(44);
    if ( la%lb != la ) e(45);
    if ( lc%ld != lc ) e(46);
    if (( (lf/ld) << la >> la ) != 2 ) e(47);
    if ( (la = lb = lc = ld = lf) != lf ) e(48);
    if ( ( la!=lb ) || ( lb!=lc ) || !(ld==lf) ) e(49);
    if ( (la<lb) || (lb>la) || (lc<lf) ) e(50);
    if ( (la%lb) || (lb%la) || (lc%ld) || (lf%ld) ) e(51);
    if ( (la%(lb<<0)  + ((lb<<3)>>3)%(la/la)) ) e(52);
    if ( ! ( la++ == lb++ ) ) e(53);
    if ( la != lb ) e(54);
    if ( la++ != (lb++) ) e(55);
    la = 1;
    lb = la + 1;
    if ( -la != -la ) e(56);
    if ( la != --lb ) e(57);
    if ( (((((la))))) != -(-(-(-(la)))) ) e(59);
    if ( lb != 1 ) e(60);
    la = 40000;
    lb = 80000;
    lc = 800000L;
    ld = -800000L;
    lf = 1200000L;
    if ( lc != -ld ) e(61);
    if ( 10 * la != 400000L ) e(62);
    if ( 2 * la != lb ) e(63);
    if ( -(-(-(-(la)))) != la ) e(64);
    if ( 10 * lb != lc ) e(65);
    if ( lf + lf != 2 * lf ) e(66);
    if ( lf - lf ) e(67);
    if ( lc / lc != 1 ) e(68);
    if ( lc / lb != 10 ) e(69);
    if ( lf / la != 30 ) e(70);
    if ( la + lb != lf / 10 ) e(71);
    if ( la - 40000 - 1 != lc - 800001L ) e(72);
    if ( la + la + la + la +la + la != lb + lb + lb ) e(73);
    if ( la > lb ) e(74);
    if ( lb > lc ) e(75);
    if ( lf < ld ) e(76);
    if ( (lf<1000000L) || (((lc-lb-lb*10)>0)) ) e(77);
    if ( lf / 01 != lf ) e(78);
    if ( lf * 01 != lf ) e(79);
    if ( lf + 'a' != lf + 'b' -1 ) e(80);
    if ( (lf % 'a') % 'a' != lf % 'a' ) e(81);
    if ( lf % lf ) e(82);
    if ( lb % la ) e(83);
    if ( (lf<<1) != lf * 2 ) e(84);
    if ( ! ( ( --lf % la ) + 1 ) ) e(86);
    if ( ( lb >> 1 ) ^ la ) e(87);
    la = 1;
    if ( la != 1 ) e(89);
    la <<= 20;
    lb = 2;
    if ( (lb<<19) != la ) e(90);
    la = lb = lc= ld = lf = -345678L;
    if ( (la != lb) || (lb != lc) || (ld != lf) ) e(91);
    if ( (la != lb) || (lb != lc) || (lc != ld) || (ld != lf) ) e(92);
    if ( la != -345678L ) e(93);
    la = 1 | 2;
    la <<= 20;
    lb = la & la & la & la & la | la | la | la;
    if ( la != lb ) e(94);
    if ( ! ( la & lb ) ) e(95);
    if ( la ^ lb ) e(96);
    if ( ! (la | lb) ) e(97);
    if ( (la >> 20) != 3 ) e(98);
    la = 20000;
    la *= 2;
    if ( la < 0 ) e(99);
    if ( 1 * la != la ) e(100);
    lb = 20000;
    if ( (lb<<1) != la ) e(101);
    if ( (5*lb)/10 != lb/2 ) e(102);
    if ( 4*lb != 1*01*2*2*lb ) e(103);
    la = lb = 30000;
    if ( la != la * lb / 30000 ) e(104);
    if ( ++la != ++lb ) e(105);
    lc = 5;
    ld = 150000L;
    if ( lc * (la-1) != ld ) e(106);
}



test6()		/* global records */
{
#ifndef NOFLOAT
    double epsd;
    float epsf;
    double fabs();
#endif

    t = 6;
    tct++;
#ifndef NOFLOAT
    epsd = EPSD;
    epsf = EPSF;
#endif
    r1.c1 = 'x';
    r1.i = 40;
    r1.j = 50;
#ifndef NOFLOAT
    r1.aaa = 3.0;
    r1.bbb = 4.0;
#endif
    r2.c1 = r1.c1;
    r2.i = 50;
    r2.j = 40;
#ifndef NOFLOAT
    r2.aaa = 4.0;
    r2.bbb = 5.0;
#endif
    if ( r1.c1 != 'x' || r1.i != 40 ) e(1);
#ifndef NOFLOAT
    if ( r1.aaa != 3.0 ) e(1);
#endif
    if ( r1.i != 40 || r2.i != 50 ) e(2);
    if ( r2.j != 40 || r1.j != 50 ) e(3);
    if ( (r1.c1 + r2.c1)/2 != 'x' ) e(4);
#ifndef NOFLOAT
    if ( r1.aaa * r1.aaa + r2.aaa * r2.aaa != r2.bbb * r2.bbb ) e(5);
    r1.i = r1.j = r2.i = r2.j = 3.0;
#else
    r1.i = r1.j = r2.i = r2.j = 3;
#endif
    if ( r1.i != 3 ) e(6);
    if ( r1.i * r2.j - 9 ) e(7);
    r1.i++;
    if ( r1.i != 4 ) e(8);
    if ( --r1.i != 3 ) e(9);
    if ( (++r2.i) * (--r2.j) != 8 ) e(10);
    if ( (r2.i = r2.j = r1.j = r1.i = -5 ) != -5 ) e(11);
    if ( r2.i * r1.j / 25 != 1 ) e(12);
    r1.c1 = '\0';
    if ( r1.i * r1.j * r2.i * r1.c1 * r2.j ) e(13);
    r2.c1 = 'j';
    if ( r1.c1 + r2.c1 != 'j' ) e(14);
    if ( r1.c1 * r2.c1 ) e(15);
     r2.j = r1.i = r2.i = r1.j = 1;
    if ( (r1.i<<0) != r1.j ) e(16);
    if ( (r1.i >> -0 ) != ( r1.j >> 0 ) ) e(17);
    if ( (r1.i<<1) != 2 ) e(18);
    if ( (r1.i<<2) != 4 ) e(19);
    if ( (r1.j<<3) != (r2.j<<3) ) e(20);
    if ( (r1.i | r1.i | r1.i | r1.i | r1.i) != r1.i ) e(21);
    if ( (r2.j & r1.j & r2.j & r2.i) != (r1.i<<3>>3) ) e(22);
    r1.j = 1;
#ifndef NOFLOAT
    r1.aaa = 2.0;
    if ( fabs ( r1.j * r1.aaa - 2.0 ) > epsd ) e(23);
    if ( (r1.j << 4) * r1.aaa != (r1.j << 4) * r1.aaa ) e(24);
    if ( ((r1.j<<6)&r1.j) * r1.aaa ) e(25);
    if ((r1.j | (r1.j << 1)) * r1.aaa != ((r1.j << 1) ^ r1.j) * r1.aaa) e(26);
#endif
    r1.i = r1.j = r2.i = r2.j = -2;
    if ( r1.i > 0 || r1.j >= 0 ) e(27);
    if ( r1.i != r2.j ) e(28);
    if ( !!! ((((( r1.i == r2.j ))))) ) e(28);
    if ( -(-(r1.j)) != r2.j ) e(29);
    if ( r1.i % r1.j ) e(30);
    if ( (r1.i % r1.j) % r1.i ) e(31);
    if ( 0 % r2.j ) e(32);
    if ( 03 * r1.i != -6 ) e(33);
#ifndef NOFLOAT
    r1.aaa = r2.aaa = -4;
    r1.bbb = r2.bbb = 4;
    if ( r1.aaa > -3.5 ) e(34);
    if ( fabs ( r1.aaa - r2.aaa ) > epsf ) e(35);
#endif
    r1.c1 = '\03';
#ifndef NOFLOAT
    if ( fabs ( r2.aaa * r1.aaa - r1.c1 * 5 - 1.0 ) > epsf ) e(36);
#else
    if ( 5*r1.c1 != 15 ) e(36) ;
#endif
}




test7()		/* local records */
{
#ifndef NOFLOAT
    double epsd;
    float epsf;
    double fabs();
#endif
    struct tp2 s1, s2;

    t = 7;
    tct++;
#ifndef NOFLOAT
    epsd = EPSD;
    epsf = EPSF;
#endif
    s1.c1 = 'x';
    s1.i = 40;
    s1.j = 50;
#ifndef NOFLOAT
    s1.aaa = 3.0;
    s1.bbb = 4.0;
#endif
    s2.c1 = s1.c1;
    s2.i = 50;
    s2.j = 40;
#ifndef NOFLOAT
    s2.aaa = 4.0;
    s2.bbb = 5.0;
#endif
    if ( s1.c1 != 'x' || s1.i != 40 ) e(1);
#ifndef NOFLOAT
    if ( s1.aaa != 3.0 ) e(1);
#endif
    if ( s1.i != 40 || s2.i != 50 ) e(2);
    if ( s2.j != 40 || s1.j != 50 ) e(3);
    if ( (s1.c1 + s2.c1)/2 != 'x' ) e(4);
#ifndef NOFLOAT
    if ( s1.aaa * s1.aaa + s2.aaa * s2.aaa != s2.bbb * s2.bbb ) e(5);
    s1.i = s1.j = s2.i = s2.j = 3.0;
#else
    s1.i = s1.j = s2.i = s2.j = 3;
#endif
    if ( s1.i != 3 ) e(6);
    if ( s1.i * s2.j - 9 ) e(7);
    s1.i++;
    if ( s1.i != 4 ) e(8);
    if ( --s1.i != 3 ) e(9);
    if ( (++s2.i) * (--s2.j) != 8 ) e(10);
    if ( (s2.i = s2.j = s1.j = s1.i = -5 ) != -5 ) e(11);
    if ( s2.i * s1.j / 25 != 1 ) e(12);
    s1.c1 = '\0';
    if ( s1.i * s1.j * s2.i * s1.c1 * s2.j ) e(13);
    s2.c1 = 'j';
    if ( s1.c1 + s2.c1 != 'j' ) e(14);
    if ( s1.c1 * s2.c1 ) e(15);
     s2.j = s1.i = s2.i = s1.j = 1;
    if ( (s1.i<<0) != s1.j ) e(16);
    if ( (s1.i >> -0 ) != ( s1.j >> 0 ) ) e(17);
    if ( (s1.i<<1) != 2 ) e(18);
    if ( (s1.i<<2) != 4 ) e(19);
    if ( (s1.j<<3) != (s2.j<<3) ) e(20);
    if ( (s1.i | s1.i | s1.i | s1.i | s1.i) != s1.i ) e(21);
    if ( (s2.j & s1.j & s2.j & s2.i) != (s1.i<<3>>3) ) e(22);
    s1.j = 1;
#ifndef NOFLOAT
    s1.aaa = 2.0;
    if ( fabs ( s1.j * s1.aaa - 2.0 ) > epsd ) e(23);
    if ( (s1.j << 4) * s1.aaa != (s1.j << 4) * s1.aaa ) e(24);
    if ( ((s1.j<<6)&s1.j) * s1.aaa ) e(25);
    if ((s1.j | (s1.j << 1)) * s1.aaa != ((s1.j << 1) ^ s1.j) * s1.aaa) e(26);
#endif
    s1.i = s1.j = s2.i = s2.j = -2;
    if ( s1.i > 0 || s1.j >= 0 ) e(27);
    if ( s1.i != s2.j ) e(28);
    if ( !!! ((((( s1.i == s2.j ))))) ) e(28);
    if ( -(-(s1.j)) != s2.j ) e(29);
    if ( s1.i % s1.j ) e(30);
    if ( (s1.i % s1.j) % s1.i ) e(31);
    if ( 0 % s2.j ) e(32);
    if ( 03 * s1.i != -6 ) e(33);
#ifndef NOFLOAT
    s1.aaa = s2.aaa = -4;
    s1.bbb = s2.bbb = 4;
    if ( s1.aaa > -3.5 ) e(34);
    if ( fabs ( s1.aaa - s2.aaa ) > epsf ) e(35);
#endif
    s1.c1 = '\03';
#ifndef NOFLOAT
    if ( fabs ( s2.aaa * s1.aaa - s1.c1 * 5 - 1.0 ) > epsf ) e(36);
#else
    if ( 5*s1.c1 != 15 ) e(36) ;
#endif
}




test8()		/* global arrays */
{
#ifndef NOFLOAT
    float epsf;
    double epsd;
    double fabs();
#endif

    t = 8;
    tct++;
#ifndef NOFLOAT
    epsf = EPSF;
    epsd = EPSD;
#endif
    for ( i=0; i<20 ; i++ )
	a1[i] = i*i;
    if ( a1[9] != 81 || a1[17] != 289 || a1[0] != 0 ) e(1);
    if ( a1[1] + a1[2] + a1[3]  !=  14 ) e(2);
    if ( ! a1[15] ) e(3);
    if ( a1[8] / a1[4] != 4 ) e(4);
#ifndef NOFLOAT
    for ( i=0; i<20; i++ )
	 a2[i] = 10.0e-1 + i/54.324e-1;
    if ( fabs(a2[4]*a2[4]-a2[4]*(10.0e-1 + 4/54.324e-1 ) ) > epsf ) e(5);
    if ( fabs(a2[8]/a2[8]*a2[9]/a2[9]-a2[10]+a2[10]-1.0 ) > epsf ) e(6);
    if ( fabs(a2[5]-a2[4]-1/54.324e-1 ) > epsf ) e(7);
    for ( i=0; i<20; i++)
	 a3[i]= 10.0e-1 + i/54.324e-1;
    if ( fabs(a3[4]*a3[4]-a3[4]*(1.0e0+4/54.324e-1 )) > epsd ) e(8);
    if ( fabs( a3[8]*a3[9]/a3[8]/a3[9]-a3[10]+a3[10]-1000e-3) > epsd ) e(9);
    if ( fabs(a3[8]+a3[6]-2*a3[7]) > epsd ) e(10);
#endif
    for ( i=0; i<20; i++ )
	a1[i] = i+1;
    if ( a1[a1[a1[a1[a1[a1[0]]]]]] != 6 ) e(11);
    if ( a1[a1[0]+a1[1]+a1[2]+a1[3]] != 11 ) e(12);
    if ( (a1[0] << 2) != 4 ) e(13);
    if ( (a1[0] >> 2) ) e(14);
    if ( (a1[0] << 3 >> 3) != a1[0] ) e(15);
    if ( a1[a1[0] << 1] != 3 ) e(16);
    if ( a1[4<<1] != 9 ) e(17);
    if ( a1[4 << 1] != 9 ) e(18);
    if ( (1 << a1[0]) != 2 ) e(19);
    if ( (1 & a1[0]) != 1 ) e(20);
    if ( a1[4]++ != 5 ) e(21);
    if ( a1[4] != 6 ) e(22);
    if ( --a1[4] != 5 ) e(23);
    if ( a1[ --a1[10] ] != 10 ) e(24);
    a1[0] = 0;
    a1[1] = 1;
    a1[2] = 2;
    a1[3] = 3;
    i = 3;
    if ( a1[--i] != 2 ) e(25);
    if ( a1[ a1[--i] ] != 1 ) e(26);
    if ( a1[a1[a1[a1[a1[a1[a1[a1[3]]]]]]]] != 3 ) e(27);
    if ( a1[1+2] != 3 ) e(28);
    if ( a1[1+2] != a1[3/3] + 2 ) e(29);
    if ( a1[i=2] != 2 ) e(30);
    if ( -a1[i==3] ) e(31);
    if ( a1[3*2 + a1[0]*6 - 10/2 -4 + 3/1] != 0 ) e(32);
    if ( a1['a' + 'c' -2*'b'] ) e(33);
    if ( a1[ a1[0]==a1[1] ] ) e(34);
    if ( a1[a1[1<<1]>>1] != 1 ) e(35);
    a1[i=j=4] = 10;
    if ( (i!=4) || (j!=4) || (i!=j) ) e(36);
    if ( a1[4] != 10 ) e(37);
    if ( a1[--i] != 3 ) e(38);
    if ( a1[i++] != 3 ) e(39);
    if ( --a1[--i] != 2 ) e(40);
    a1[a1[a1[a1[a1[0]=7]=5]=8]=2]=0;
    if ( a1[0] != 7) e(41);
    if ((a1[7] != 5) || (a1[5]!=8) || (a1[8]!=2))e(42);
    if (a1[2]) e(43);
    for ( i=0 ; i<20; i++)
	a1[i] = i;
    a1[0] = 0;
    a1[1] = 01;
    a1[2] = 02;
    a1[3] = 04;
    a1[4] = 010;
    if ((a1[0] | a1[1] | a1[2] | a1[3] | a1[4]) != 017 ) e(44);
    if ( a1[0]<<4 ) e(45);
    if ( (a1[4]>>3) != 1 ) e(46);
    a1[4] = 04;
    a1[010] = 010;
    if ( a1[8] != 8 ) e(47);
    if ( a1[0|1|2|4|8] != (a1[0]|a1[1]|a1[2]|a1[4]|a1[8]) ) e(48);
    if ( a1[a1[0]|a1[1]|a1[2]|a1[4]|a1[8]] != a1[017] ) e(49);
    if ( a1[a1[1]^a1[2]^a1[4]^a1[8]] != a1[a1[1]|a1[2]|a1[4]|a1[8]] ) e(50);
    for ( i = 0; i<20; i++ )
	a1[i] = i+1;
#ifndef NOFLOAT
    for ( i = 0; i<20; i++ )
	a2[i] = a3[i] = a1[i];
    if ( a2[5] != 6.0 ) e(51);
    if ( a2[13] != 14.0 ) e(52);
    if ( a2[a1[a1[a1[a1[a1[0]]]]]] != 6.0 ) e(53);
#endif
    if ( a1[12] != 13 ) e(54);
#ifndef NOFLOAT
    if ( a1[ a1[12] = a2[a1[11]] ] != 14 ) e(55);
    if ( fabs( a2[13] - a2[a1[12]] ) > epsf ) e(56);
    if ( a2[8] != a1[8] ) e(57);
#endif
}




test9()	/* local arrays */
{
#ifndef NOFLOAT
    float epsf;
    double epsd;
    double fabs();
#endif
    int b1[20];
#ifndef NOFLOAT
    float b2[20];
    double b3[20];
#endif

    t = 9;
    tct++;
#ifndef NOFLOAT
    epsf = EPSF;
    epsd = EPSD;
#endif
    for ( i=0; i<20 ; i++ )
	b1[i] = i*i;
    if ( b1[9] != 81 || b1[17] != 289 || b1[0] != 0 ) e(1);
    if ( b1[1] + b1[2] + b1[3]  !=  14 ) e(2);
    if ( ! b1[15] ) e(3);
    if ( b1[8] / b1[4] != 4 ) e(4);
#ifndef NOFLOAT
    for ( i=0; i<20; i++ )
	 b2[i] = 10.0e-1 + i/54.324e-1;
    if ( fabs(b2[4]*b2[4]-b2[4]*(10.0e-1 + 4/54.324e-1 ) ) > epsf ) e(5);
    if ( fabs(b2[8]/b2[8]*b2[9]/b2[9]-b2[10]+b2[10]-1.0 ) > epsf ) e(6);
    if ( fabs(b2[5]-b2[4]-1/54.324e-1 ) > epsf ) e(7);
    for ( i=0; i<20; i++)
	 b3[i]= 10.0e-1 + i/54.324e-1;
    if ( fabs(b3[4]*b3[4]-b3[4]*(1.0e0+4/54.324e-1 )) > epsd ) e(8);
    if ( fabs( b3[8]*b3[9]/b3[8]/b3[9]-b3[10]+b3[10]-1000e-3) > epsd ) e(9);
    if ( fabs(b3[8]+b3[6]-2*b3[7]) > epsd ) e(10);
#endif
    for ( i=0; i<20; i++ )
	b1[i] = i+1;
    if ( b1[b1[b1[b1[b1[b1[0]]]]]] != 6 ) e(11);
    if ( b1[b1[0]+b1[1]+b1[2]+b1[3]] != 11 ) e(12);
    if ( (b1[0] << 2) != 4 ) e(13);
    if ( (b1[0] >> 2) ) e(14);
    if ( (b1[0] << 3 >> 3) != b1[0] ) e(15);
    if ( b1[b1[0] << 1] != 3 ) e(16);
    if ( b1[4<<1] != 9 ) e(17);
    if ( b1[4 << 1] != 9 ) e(18);
    if ( (1 << b1[0]) != 2 ) e(19);
    if ( (1 & b1[0]) != 1 ) e(20);
    if ( b1[4]++ != 5 ) e(21);
    if ( b1[4] != 6 ) e(22);
    if ( --b1[4] != 5 ) e(23);
    if ( b1[ --b1[10] ] != 10 ) e(24);
    b1[0] = 0;
    b1[1] = 1;
    b1[2] = 2;
    b1[3] = 3;
    i = 3;
    if ( b1[--i] != 2 ) e(25);
    if ( b1[ b1[--i] ] != 1 ) e(26);
    if ( b1[b1[b1[b1[b1[b1[b1[b1[3]]]]]]]] != 3 ) e(27);
    if ( b1[1+2] != 3 ) e(28);
    if ( b1[1+2] != b1[3/3] + 2 ) e(29);
    if ( b1[i=2] != 2 ) e(30);
    if ( -b1[i==3] ) e(31);
    if ( b1[3*2 + b1[0]*6 - 10/2 -4 + 3/1] != 0 ) e(32);
    if ( b1['a' + 'c' -2*'b'] ) e(33);
    if ( b1[ b1[0]==b1[1] ] ) e(34);
    if ( b1[b1[1<<1]>>1] != 1 ) e(35);
    b1[i=j=4] = 10;
    if ( (i!=4) || (j!=4) || (i!=j) ) e(36);
    if ( b1[4] != 10 ) e(37);
    if ( b1[--i] != 3 ) e(38);
    if ( b1[i++] != 3 ) e(39);
    if ( --b1[--i] != 2 ) e(40);
    b1[b1[b1[b1[b1[0]=7]=5]=8]=2]=0;
    if ( b1[0] != 7) e(41);
    if ((b1[7] != 5) || (b1[5]!=8) || (b1[8]!=2))e(42);
    if (b1[2]) e(43);
    for ( i=0 ; i<20; i++)
	b1[i] = i;
    b1[0] = 0;
    b1[1] = 01;
    b1[2] = 02;
    b1[3] = 04;
    b1[4] = 010;
    if ((b1[0] | b1[1] | b1[2] | b1[3] | b1[4]) != 017 ) e(44);
    if ( b1[0]<<4 ) e(45);
    if ( (b1[4]>>3) != 1 ) e(46);
    b1[4] = 04;
    b1[010] = 010;
    if ( b1[8] != 8 ) e(47);
    if ( b1[0|1|2|4|8] != (b1[0]|b1[1]|b1[2]|b1[4]|b1[8]) ) e(48);
    if ( b1[b1[0]|b1[1]|b1[2]|b1[4]|b1[8]] != b1[017] ) e(49);
    if ( b1[b1[1]^b1[2]^b1[4]^b1[8]] != b1[b1[1]|b1[2]|b1[4]|b1[8]] ) e(50);
    for ( i = 0; i<20; i++ )
	b1[i] = i+1;
#ifndef NOFLOAT
    for ( i = 0; i<20; i++ )
	b2[i] = b3[i] = b1[i];
    if ( b2[5] != 6.0 ) e(51);
    if ( b2[13] != 14.0 ) e(52);
    if ( b2[b1[b1[b1[b1[b1[0]]]]]] != 6.0 ) e(53);
#endif
    if ( b1[12] != 13 ) e(54);
#ifndef NOFLOAT
    if ( b1[ b1[12] = b2[b1[11]] ] != 14 ) e(55);
    if ( fabs( b2[13] - b2[b1[12]] ) > epsf ) e(56);
    if ( b2[8] != b1[8] ) e(57);
#endif
}




test10()		/* global pointers */
{
#ifndef NOFLOAT
    float epsf;
    double fabs();
#endif
    int li;
    struct tp2 strp2;

#ifndef NOFLOAT
    epsf = EPSF;
#endif
    t = 10;
    tct++;
    p1 = &li;
    li = 076;
    if ( p1 != &li ) e(1);
    p11 = &li;
    if ( p1 != p11 ) e(3);
    if ( *p1 != *p11 ) e(4);
    if ( &li != p11 ) e(5);
    if ( *&p1 != p1 ) e(6);
    if ( &*p1 != p1 ) e(7);
    if ( **&p1 != *&*p1 ) e(10);
    if ( *&*&*&*&*&li != li ) e(11);
    p1 = &p ;
    p2 = &p1;
    *p1 = **p2 = 34;
    if ( p1 != *p2 ) e(25);
    li = 4;
    p1 = &li;
    p2 = &p1;
    p3 = &p2;
    p4 = &p3;
    p5 = &p4;
    if ( *p1 != **p2 ) e(26);
    if ( **p2 != **p2 ) e(27);
    if ( ***p3 != **p2 ) e(28);
    if ( *****p5 != 4 ) e(30);
    li = 3;
    if ( *p1 - *p1 ) e(44);
    if ( p1 != &li ) e(46);
    pp1 = (struct tp2 *) alloc( sizeof *pp1 );
    pp2 = (struct tp2 *) alloc( sizeof *pp2 );
    pp3 = (struct tp2 *) alloc( sizeof *pp3 );
    pp1->i = 1325;
    if ( pp1->i != 1325 ) e(47);
    pp1->i = pp2->i = pp3->i = 3;
    if ( pp1->i * pp1->i != 9 ) e(48);
    if ( pp1->i * pp2->i * pp3->i != pp2->i * 3 * 3 ) e(49);
    if ( pp1->i - pp3->i ) e(50);
    if ( (*pp1).i != pp1->i ) e(51);
    pp1->i++;
    if ( ++pp2->i != pp1->i ) e(52);
    if ( pp2->i != 4 ) e(53);
#ifndef NOFLOAT
    pp1->aaa = 3.0;
    pp2->aaa = -3.0;
    pp3->bbb = 25.0;
    if ( pp1->aaa != 3.0 ) e(54);
    if ( fabs( pp1->aaa + pp2->aaa ) > epsf ) e(55);
    if ( fabs( pp1->aaa * pp2->aaa + pp3->bbb - 16 ) > epsf ) e(56);
    if ( fabs( pp1->aaa / pp2->aaa + 1 ) > epsf ) e(57);
#endif
    pp1->c1 = 'x';
    pp1->i = pp1->j = 45;
#ifndef NOFLOAT
    pp1->aaa = 100.0;
    pp1->bbb = 1024.0;
#endif
    strp2.c1 = pp1->c1;		/* strp2 is a local struct */
    strp2.i = pp1->i = strp2.j = pp1->j;
#ifndef NOFLOAT
    strp2.aaa = pp1->aaa;
    strp2.bbb = pp1->bbb;
#endif
    if ( strp2.c1 != 'x' ) e(58);
    if ( strp2.i != strp2.j ) e(59);
#ifndef NOFLOAT
    if ( strp2.aaa != pp1->aaa ) e(60);
    if ( strp2.bbb != pp1->bbb ) e(61);
#endif
}

#ifndef NOFLOAT

test11()	/* real arithmetic */
{
    double fabs();
    double epsd;
    float epsf;
    float locxf;

    t = 11 ;
    tct++;
    epsf = EPSF;
    epsd = EPSD;
    xf = 1.50;
    yf = 3.00;
    zf = 0.10;
    xd = 1.50;
    yd = 3.00;
    zd = 0.10;
    if ( fabs(1.0 + 1.0 - 2.0) > epsd ) e(1);
    if ( fabs( 1e10-1e10 ) > epsd ) e(2);
    if ( fabs( 1.0e+5 * 1.0e+5 - 100e+8 ) > epsd ) e(3);
    if ( fabs( 10.0/3.0 * 3.0/10.0 - 100e-2 ) > epsd ) e(4);
    if ( 0.0e0 != 0 ) e(5);
    if ( fabs( 32767.0 - 32767 ) > epsd ) e(6);
    if ( fabs( 1.0+2+5+3.0e0+7.5e+1+140e-1-100.0 ) > epsd ) e(7);
    if ( fabs(-1+(-1)+(-1.0)+(-1.0e0)+(-1.0e-0)+(-1e0)+6 ) > epsd ) e(8);
    if ( fabs(5.0*yf*zf-xf) > epsf ) e(9);
    if ( fabs(5.0*yd*zd-xd) > epsd ) e(10);
    if ( fabs(yd*yd - (2.0*xd)*(2.0*xd) ) > epsd ) e(11);
    if ( fabs(yf*yf - (2.0*xf)*(2.0*xf) ) > epsf ) e(12);
    if ( fabs( yd*yd+zd*zd+2.0*yd*zd-(yd+zd)*(zd+yd) ) > epsf ) e(13);
    if ( fabs( yf*yf+zf*zf+2.0*yf*zf-(yf+zf)*(zf+yf) ) > epsf ) e(14);
    xf = 1.10;
    yf = 1.20;
    if ( yd<xd ) e(15);
    if ( yd<=xd ) e(16);
    if ( yd==xd ) e(17);
    if ( xd>=yd ) e(18);
    if ( yd<xd ) e(19);
    if ( fabs(yd-xd-1.5) > epsd ) e(20);
    if ( 1.0 * 3.0 != 3.0 * 1.0 ) e(21);
    if ( 1.0 != 1e+0 ) e(22);
    if ( 4.5 < 4.4 ) e(23);
    if ( -3.4 != -3.4 ) e(24);
    if ( 10/3.0 - 10/3.0 != 0.0 ) e(25);
    if ( fabs( (1<<0) * (-5.3) + 5.3 ) > epsd ) e(26);
    if ( fabs( (1<<3) * 5.0 - 4e+1 ) > epsd ) e(27);
    if ( fabs( ((1<<5)>>5) - 1e-0 ) > epsd ) e(28);
    if ( fabs ( 00000 * 3.0 ) > epsd ) e(29);
    if ( fabs ( 8 * 5.0 - 02 * 02 + 04 / 1.0 -40.0 ) > epsd ) e(30);
    if ( fabs ( 'a' / 1.0 - 'a' ) > epsd ) e(31);
    if ( fabs ( (!1) * ( 2.0 / -34e-1 ) ) > epsd ) e(32);
    if ( fabs ( (01 | 1 | 2) * 4.0 - 12.0 ) > epsd ) e(33);
    if ( fabs ( 1.0 * 2.0 * 3.0 * 4.0 * 5.0 - 120.0 ) > epsd ) e(34);
    if ( fabs ( 1.0 * 2.0 * (1 | (4>>1)) - 6 ) > epsd ) e(35);
    if ( fabs ( ( 0 ^ 0 ^ 0 ^ 0 ) * 0.0 ) > epsd ) e(36);
    if ( fabs ( 1.0 * 2.0 * (1 ^ (4>>1)) - 6 ) > epsd ) e(37);
    if ( fabs ( (((((-1.0 * (((((-1.0))))) - 1.0 ))))) ) > epsd) e(38);
    if ( fabs ( ( 2==3 ) * 3.0  ) > epsd ) e(39);
    if ( ( 4 + 3 > 5 ? 3.4 : -5e+3 ) != 3.4 ) e(40);
    if ( ( -4 -'a' > 0 ? 3.4 : -5e+3 ) != -5e+3 ) e(41);
    locxf = 3.0;
    xf = 3.0;
    if ( locxf != locxf ) e(42);
    if ( locxf != xf ) e(43);
    if ( locxf * xf != xf * locxf ) e(44);
    if ( fabs ( ((2*3)>>1) / 3.0 - 1.0 ) > epsd ) e(45);
    if ( fabs ( 'a' / locxf - 'a' / xf ) > epsd ) e(46);
    if ( fabs( xf * locxf - 9.0 ) > epsd ) e(47);
    yd = 3.0;
    if ( fabs( xf*yd - 9.0) > epsd ) e(48);
    if ( yd >= 4 ) e(49);
    if ( locxf == 2 ) e(50);
}

#endif


