/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

char rcs_id[] = "$Id$" ;

/* This program can be used to test C-compilers     */

int i,j,k,l,m,ect,pct,t,*p1;
int a1[20];
#ifndef NOFLOAT
float a2[20],xf,yf,zf;
double a3[20],xd,yd,zd;
#endif

char alstr[3000] ;
char *alptr = alstr ;

struct tp2
{ char c1;
  int i,j;
#ifndef NOFLOAT
  float aaa;
  double bbb;
#endif
} r1,r2,*p3;

struct node
{ int val;
 struct node *next;
} *head,*tail,*p2;

main()
{ ect = 0; pct = 0;
  test1();test2();test3();
  test4();test5();
  test6();test7();test8();
  test9();test10();
#ifndef NOFLOAT
  test11();
#endif
  printf("program test1\n");
  printf("%d tests completed. Number of errors = %d\n",pct,ect);
	return 0 ;
}

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

int abs(a) int a ; { return ( a<0 ? -a : a) ; }
#ifndef NOFLOAT
double fabs(a) double a ; { return( a<0 ? -a : a) ; }
#endif

e(n)
{ ect++; printf("error %d in test %d \n",n,t);
}

inc(n)
{ return(++n);}

/***********************************************************************/

test1()
/*arithmetic on constants */
{ t = 1; pct++;
  if (1+1 != 2) e(1);
  if (3333 + 258 != 3591) e(2);
  if (3*4 != 12) e(3);
  if (111*111 != 12321) e(4);
  if (50 / 5 != 10) e(5);
  if (7498 / 75 != 99) e(6);
  if (456 - 345 != 111) e(7);
  if (1+(-2) != -1) e(8);
  if (-3 * -4 != 12) e(9);
  if (-2 / 2 != -1) e(10);
  if (-5 / 1 != -5 ) e(11);
  if (-4 - -5 != 1) e(12);
  if ( 03 + 02 != 05) e(13);
  if ( 03456 + 88 != 03606 ) e(14);
  if ( 0100 * 23 != 02700 ) e(15);
  if ( 045 / 020 != 2) e(16);
  if (0472 - 0377 != 073 ) e(17);
  if ('a' + 3 != 100) e(18);
  if ('a' + 'c' != 'b' + 'b') e(19);
  if ( 'z' * 'z' != 14884 ) e(20);
  if ( -'z' / 01 != -'z' ) e(21);
  if ( 077777 >> 3 != 07777 ) e(22);
  if ( 077777 >> 15 ) e(23);
  if ( 234 << 6 != 234 << 6 ) e(24);
  if ( 0124 & 07765 != 0124 ) e(25);
  if ( 34 & 31 != 2 ) e(26);
  if ( ( -4 | 3 ) != -1 ) e(27);
  if ( ( 5 | 013 | 020 ) != 31 ) e(28);
  if ( ( -7 ^ 3 ) != -6 ) e(29);
  if ( ( 07373 ^ 4632 ) != 016343 ) e(30);
  if ( (1+2+3)*(2+3+4)*(3+5+5) / 2 != ((3*((5+3+2)*10)+51)*6)/6 ) e(31);
  if ( (1000*2+5*7+13)/ 8 != 2*2*2*2*4*4 ) e(32);
  if ((1*2*3*4*5*6*7 / 5040 !=
                 5040 / 7 / 6 / 5 / 4 / 3 / 2 / 1 )) e(33);
  if ( -(-(-(-(-(-(1)))))) != 1 ) e(34);
  if (-                    1     !=  -((((((((((1)))))))))) ) e(35);
  if ( -1-1-1-1-1-1       !=       -6-3+3 ) e(36);
  if (  -4 * -5 != 20 ) e(37);  
  if ( 2<1 ) e(38);
  if ( 2<= 1 ) e(39);
  if ( 2==3 ) e(40);
  if ( 2 != 2 ) e(41);
  if ( 2 >= 3) e(42);
  if ( 2 > 3 ) e(43);
  if (2 + 0 != 2 ) e(44);
  if (2 - 0 != 2 ) e(45);
  if (2 * 0 != 0 ) e(46);
  if ( 0 / 1 != 0 ) e(47);
  if ( -0 != 0 ) e(48);
  if ( 0 * 0 != 0 ) e(49);
  if ( 32767 > 32767 ) e(50);
  if ( 0456 < 0400 ) e(51);
  if ( 0456 != ( 0400 | 050 | 06 ) ) e(52);
  if ( 2*2<<2*2/4 != 010 ) e(53);
  if ( 0 || 0 ) e(54);
  if ( 1 && 0 ) e(55);
  if ( ( 123 ? 123 * 4 :345) != 492 ) e(56);
  if ( ( 0 ? 345 : 280) != 280 ) e(57);
  if ( ( (2*2/2<<2)|(2/2) ) != 9 ) e(58); 
  if ( !( 111 || 23 && 0 ) ) e(59);
  if ( !1 ) e(60);
  if ( !0 == 0 ) e(61);
  if ( !!!!!!!!0 ) e(62);
}

/***********************************************************************/

test2()
/*arithmetic on global integer variables*/
{ t = 2;  pct++;
  i = 1; j = 2; k = 3; l = 4; m = 10;
  if ( i+j != k ) e(1);
  if ( i+k != l ) e(2);
  if ( j-k != -i ) e(3);
  if ( j*(j + k) != m ) e(4);
  if ( -m != -(k+k+l) ) e(5);
  if ( i / i != 1 ) e(6);
  if ( m*m / m != m ) e(7);
  if ( 10 * m != 100 ) e(8);
  if ( m * (-10) != -100 ) e(9);
  if ( j / k != 0 ) e(10);
  if ( 100 / k != 33 ) e(11);
  if ( i+j*k+l+m / j + 50 / k != 32 ) e(12);
  if ( j*k*m / 6 != 10 ) e(13);
  if ( (k>4) || (k>=4) || (k==4) ) e(14);
  if ( (m<j) || (m<=j) || (m==j) ) e(15);
  if ( i+j-k ) e(16);
  if ( j<i ) e(17);
  if ( j != j ) e(18);
  if ( i > j ) e(19);
  if ( (i > j ? k : k*j ) != 6 ) e(20);
  if ( (i < j ? k : k*j ) != 3 ) e(21);
  if ( j<<i != l ) e(22);
  if ( j>> i != i ) e(25);
  if ( i++ != 1 ) e(26);
  if ( --i != 1 ) e(27);
  if ( i-- != 1 ) e(28);
  if ( ( i+j ) && ( i<0 ) || (m-10) && (064) ) e(29);
  if ( ( i+j ) &&  !(i>=0) || (m-10) && !( 0 ) ) e(30);
}

/***********************************************************************/

test3()
/*arithmetic on local integer variables*/
{ int a,b,c,d,f;
  t = 3;  pct++;
  a = 1; b = 2; c = 3; d = 4; f = 10;
  if ( a+b != c ) e(1);
  if ( a+c != d ) e(2);
  if ( b-c != -a ) e(3);
  if ( b*(b + c) != f ) e(4);
  if ( -f != -(c+c+d) ) e(5);
  if ( a / a != 1 ) e(6);
  if ( f*f / f != f ) e(7);
  if ( 10 * f != 100 ) e(8);
  if ( f * (-10) != -100 ) e(9);
  if ( b / c != 0 ) e(10);
  if ( 100 / c != 33 ) e(11);
  if ( a+b*c+d+f / b + 50 / c != 32 ) e(12);
  if ( b*c*f / 6 != 10 ) e(13);
  if ( (c>4) || (c>=4) || (c==4) ) e(14);
  if ( (f<b) || (f<=b) || (f==b) ) e(15);
  if ( c != a+b ) e(16);
  if ( b<a ) e(17);
  if ( b != b ) e(18);
  if ( a > b ) e(19);
  if ( (a > b ? c : c*b ) != 6 ) e(20);
  if ( (a < b ? c : c*b ) != 3 ) e(21);
  if ( b<<a != d ) e(22);
  if ( b>> a != a ) e(25);
  if ( a++ != 1 ) e(26);
  if ( --a != 1 ) e(27);
  if ( a-- != 1 ) e(28);
  if ( ( a+b ) && ( a<0 ) || (f-10) && (064) ) e(29);
  if ( ( a+b ) &&  !(a>=0) || (f-10) && !( 0 ) ) e(30);
}

/***********************************************************************/

test4()
/* global arrays */
{
#ifndef NOFLOAT
  float epsf;
  double epsd;
#endif
  t=4; pct++;
#ifndef NOFLOAT
  epsf = 1e-7; epsd = 1e-14;
#endif
  for ( i=0; i<20 ; i++ ) a1[i] = i*i;
  if ( a1[9] != 81 || a1[17] != 289 || a1[0] != 0 ) e(1);
  if ( a1[1] + a1[2] + a1[3]  !=  14 ) e(2);
  if ( ! a1[15] ) e(3);
  if ( a1[8] / a1[4] != 4 ) e(4);
#ifndef NOFLOAT
  for ( i=0; i<20; i++ ) a2[i] = 10.0e-1 + i/54.324e-1;
  if ( fabs(a2[4]*a2[4]-a2[4]*(10.0e-1 + 4/54.324e-1 ) ) > epsf ) e(5);
  if ( fabs(a2[8]/a2[8]*a2[9]/a2[9]-a2[10]+a2[10]-1.0 ) > epsf ) e(6);
  if ( fabs(a2[5]-a2[4]-1/54.324e-1 ) > epsf ) e(7);
  for ( i=0; i<20; i++ ) a3[i]= 10.0e-1 + i/54.324e-1;
  if ( fabs(a3[4]*a3[4]-a3[4]*(1.0e0+4/54.324e-1 )) > epsd ) e(8);
  if ( fabs( a3[8]*a3[9]/a3[8]/a3[9]-a3[10]+a3[10]-1000e-3) > epsd ) e(9);
  if ( fabs(a3[8]+a3[6]-2*a3[7]) > epsd ) e(10);
#endif
}

/****************************************************************/

test5()
/* local arrays */
{ int b1[20];
#ifndef NOFLOAT
  float epsf, b2[20]; double b3[20],epsd;
  epsf = 1e-7; epsd = 1e-14;
#endif
  t = 5; pct++;
  for ( i=0; i<20 ; i++ ) b1[i] = i*i;
  if ( b1[9]-b1[8] != 17 ) e(1);
  if ( b1[3] + b1[4] != b1[5] ) e(2);
  if ( b1[1] != 1||b1[3] != 9 || b1[5] != 25 || b1[7] != 49 ) e(3);
  if ( b1[12] / b1[6] != 4   ) e(4);
#ifndef NOFLOAT
  for ( i=0; i<20; i += 1) b2[i] = 10.0e-1+i/54.324e-1;
  if (fabs(b2[4]*b2[4]-b2[4]*(10.0e-1+4/54.324e-1)) > epsf ) e(5);
  if (fabs(b2[8]/b2[8]*b2[9]/b2[9]-b2[10]+b2[10]-1.0) > epsf ) e(6);
  if ( fabs(b2[5]-b2[4]-1/5.4324 ) > epsf ) e(7);
  for ( i=0; i<20 ; i += 1 ) b3[i] = 10.0e-1+i/54.324e-1;
  if (fabs(b3[4]*b3[4]-b3[4]*(10.0e-1+4/54.324e-1)) > epsd ) e(8);
  if (fabs(b3[8]*b3[9]/b3[8]/b3[9]+b3[10]-b3[10]-1.0) > epsd ) e(9);
  if (fabs(b3[10]+b3[18]-2*b3[14]) > epsd ) e(10);
#endif
}


/****************************************************************/



test6()
/* mixed local and global */
{ int li,b1[20];
#ifndef NOFLOAT
  double b3[10],xxd,epsd;
#endif
  t = 6;  pct++;
#ifndef NOFLOAT
  epsd = 1e-14;
#endif
  li = 6; i = li ;
  if ( i != 6 ) e(1);
  i = 6; li = i;
  if ( i != li ) e(2);
  if ( i % li ) e(3);
  i=li=i=li=i=li=i=i=i=li=j;
  if ( i != li || i != j ) e(4);
  for ( i=li=0; i<20 ; i=li ) { b1[li]= (li+1)*(i+1) ; li++; }
  if ( b1[9] != a1[10] ) e(5);
  if ( b1[7]/a1[4] != a1[2] ) e(6);
  li = i = 121;
  if ( b1[10] != i && a1[11]!= li ) e(7);
#ifndef NOFLOAT
  for ( li=0 ; li<10; li++ ) b3[li]= 1.0e0 + li/54.324e-1;
  if ( fabs(b3[9]-a3[9]) > epsd ) e(8);
  if ( fabs(8/54.324e-1 - b3[9]+a3[1] ) > epsd ) e(9);
#endif
}

/***************************************************************/


test7()
/*global records */
{ t=7; pct++;
  r1.c1= 'x';r1.i=40;r1.j=50;
#ifndef NOFLOAT
  r1.aaa=3.0;r1.bbb=4.0;
#endif
  r2.c1=r1.c1;
  r2.i= 50;
  r2.j=40;
#ifndef NOFLOAT
  r2.aaa=4.0;r2.bbb=5.0;
#endif
  if (r1.c1 != 'x' || r1.i != 40 ) e(1);
#ifndef NOFLOAT
  if ( r1.aaa != 3.0 ) e(1);
#endif
  i = 25;j=75;
  if (r1.i != 40 || r2.i != 50 ) e(2);
  if ( r2.j != 40 || r1.j != 50 ) e(3);
  if ( (r1.c1 + r2.c1)/2 != 'x' ) e(4);
#ifndef NOFLOAT
  if ( r1.aaa*r1.aaa+r2.aaa*r2.aaa != r2.bbb*r2.bbb) e(5);
#endif
  r1.i = 34; if ( i!=25 ) e(6);
}


/****************************************************************/


test8()
/*local records */
{ struct tp2  s1,s2;
  t=8; pct++;
  s1.c1= 'x';s1.i=40;s1.j=50;
#ifndef NOFLOAT
  s1.aaa=3.0;s1.bbb=4.0;
#endif
  s2.c1=s1.c1;
  s2.i= 50;
  s2.j=40;
#ifndef NOFLOAT
  s2.aaa=4.0;s2.bbb=5.0;
#endif
  if (s1.c1 != 'x' || s1.i != 40 ) e(1);
#ifndef NOFLOAT
  if ( s1.aaa != 3.0 ) e(1);
#endif
  i = 25;j=75;
  if (s1.i != 40 || s2.i != 50 ) e(2);
  if ( s2.j != 40 || s1.j != 50 ) e(3);
  if ( (s1.c1 + s2.c1)/2 != 'x' ) e(4);
#ifndef NOFLOAT
  if ( s1.aaa*s1.aaa+s2.aaa*s2.aaa != s2.bbb*s2.bbb) e(5);
#endif
  s1.i = 34; if ( i!=25 ) e(6);
}



/***********************************************************************/
test9()
/*global pointers */
{ t=9; pct++;
  p1=alloc( sizeof *p1 );
  p2=alloc( sizeof *p2);
  p3=alloc(sizeof *p3);
  *p1 = 1066;
  if ( *p1 != 1066 ) e(1);
  p3->i = 1215;
  if ( p3->i != 1215 ) e(2);
  p2->val = 1566;
  if ( p2->val != 1566 || p2->next ) e(3);
  if ( a1 != &a1[0] ) e(4);
  p1 = a1;
  if ( ++p1 != &a1[1] ) e(5);
  head = 0;
  for (i=0;i<=100;i += 1)
  { tail = alloc(sizeof *p2);
    tail->val = 100+i;tail->next = head;
    head = tail;
  }
  if ( tail->val != 200 || tail->next->val != 199 ) e(6);
  if ( tail->next->next->next->next->next->val != 195) e(7);
  tail->next->next->next->next->next->val = 1;
  if ( tail->next->next->next->next->next->val != 1) e(8);
  i = 27;
  if ( *&i != 27 ) e(9);
  if ( &*&*&*&i != &i ) e(10);
  p1 = &i;i++;
  if ( p1 != &i ) e(11);
}

/*****************************************************************/
test10()
/*local pointers */
{ struct tp2 *pp3;
  struct node *pp2,*ingang,*uitgang;
  int *pp1;
  int b1[20];
  t=10; pct++;
  pp1=alloc( sizeof *pp1 );
  pp2=alloc( sizeof *p2);
  pp3=alloc(sizeof *pp3);
  *pp1 = 1066;
  if ( *pp1 != 1066 ) e(1);
  pp3->i = 1215;
  if ( pp3->i != 1215 ) e(2);
  pp2->val = 1566;
  if ( pp2->val != 1566 || p2->next ) e(3);
  if ( b1 != &b1[0] ) e(4);
  pp1 = b1;
  if ( ++pp1 != &b1[1] ) e(5);
  ingang = 0;
  for (i=0;i<=100;i += 1)
  { uitgang = alloc(sizeof *pp2);
    uitgang->val = 100+i;uitgang->next = ingang;
    ingang = uitgang;
  }
  if ( uitgang->val != 200 || uitgang->next->val != 199 ) e(6);
  if ( uitgang->next->next->next->next->next->val != 195 ) e(7);
  uitgang->next->next->next->next->next->val = 1;
  if ( uitgang->next->next->next->next->next->val != 1) e(8);
}

/***************************************************************/

#ifndef NOFLOAT
test11()
/* real arithmetic  */
{
  double epsd; float epsf;
  t = 11; pct++; epsf = 1e-7; epsd = 1e-16;
  xf = 1.50 ; yf = 3.00 ; zf = 0.10;
  xd = 1.50 ; yd = 3.00 ; zd = 0.10;
  if ( fabs(1.0 + 1.0 - 2.0 ) > epsd ) e(1);
  if ( fabs( 1e10-1e10 ) > epsd ) e(2);
  if ( fabs( 1.0e+5*1.0e+5-100e+8 ) > epsd ) e(3);
  if ( fabs( 10.0/3.0*3.0/10.0-100e-2 ) > epsd ) e(4);
  if ( 0.0e0 != 0 ) e(5);
  if ( fabs( 32767.0 - 32767 ) > epsd ) e(6);
  if ( fabs( 1.0+2+5+3.0e0+7.5e+1+140e-1-100.0 ) > epsd ) e(7);
  if ( fabs(-1+(-1)+(-1.0)+(-1.0e0)+(-1.0e-0)+(-1e0)+6 ) > epsd ) e(8);
  if ( fabs(5.0*yf*zf-xf) > epsf ) e(9);
  if ( fabs(5.0*yd*zd-xd) > epsd ) e(10);
  if ( fabs(yd*yd - (2.0*xd)*(2.0*xd) ) > epsd ) e(11);
  if ( fabs(yf*yf - (2.0*xf)*(2.0*xf) ) > epsf ) e(12);
  if ( fabs( yd*yd+zd*zd+2.0*yd*zd-(yd+zd)*(zd+yd) ) > epsd ) e(13);
  if ( fabs( yf*yf+zf*zf+2.0*yf*zf-(yf+zf)*(zf+yf) ) > epsf ) e(14);
  xf=1.10;yf=1.20;
  if ( yd<xd ) e(15);
  if ( yd<=xd ) e(16);
  if ( yd==xd ) e(17);
  if ( xd>=yd ) e(18);
  if ( yd<xd ) e(19);
  if ( fabs(yd-xd-1.5) > epsd ) e(20);
}
#endif


/*****************************************************************/
