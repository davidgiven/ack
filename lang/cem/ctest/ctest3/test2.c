/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

char rcs_id[] = "$Id$" ;

/* This program can be used to test C-compilers */


int t, ect, tct;


/**********************************************************************/
/*
 * Testing basic function calls
 *
 */



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




one()
{
    return(1);
}




two()
{
    return(2);
}




three()
{
    return(3);
}




four()
{
    return(4);
}




five()
{
    return(5);
}




plus()
{
    return ( one() + two() + three() + four() + five() );
}




multipl()
{
    return( one() * two() * three() * four() * five() );
}




subtr()
{
    return( - one() - two() - three() - four() - five() );
}




test1()
{
    int i;
    int count;

    t = 1;
    tct++;
    if ( one() != 1 ) e(1);
    if ( two() != 2 ) e(2);
    if ( three() != 3 ) e(3);
    if ( four() != 4 ) e(4);
    if ( five() != 5 ) e(5);
    if ( (one() + two()) != 3 ) e(6);
    if ( ((((((one() + two())))))) != 3) e(7);
    if ( (one() * three()) != 3) e(8);
    if (( (four() + three()) * two()) != 14) e(9);
    if ( (four() + four()) != (two() * four()) ) e(10);
    if ( (four() - four()) / three() ) e(11);
    if (( four() + 3 * 12 - ( one() * two() * 2 ) ) != 36 ) e(12);
    if ( one() & two() & four() & three() ) e(13);
    if ( !( three() && two() ) ) e(14);
    for (i=0; i<8; i++)
    {
       count = one() + two() + three() + four();
       count = count * one();
       count = count * two() - one() - two() - three() - four();
    }
    if (count != 10) e(15);
    if ( !one() ) e(16);
    if ( plus() != 15 ) e(17);
    if ( multipl() != 120 ) e(18);
    if ( subtr() != -15 ) e(19);
    if ( -subtr() != plus() ) e(18);
    if ( -subtr() != plus() ) e(21);
}




echo(a)
int a;
{
    return ( a );
}




min(a,b)
int a,b;
{
    if ( a < b )
	return(a);
    return(b);
}




max1(a,b)
int a,b;
{
    if ( a < b )
	return(b);
    return(a);
}




max2(a,b)
int a,b;
{
    return ( ( a < b ? b : a ) );
}




test2()
{
    int i,j;
    int a,b;

    t = 2;
    tct++;
    if ( echo(1) != 1 ) e(1);
    if ( echo(3) != 3 ) e(2);
    if ( echo(0) ) e(3);
    if ( echo(2) + echo(3) != echo(5) ) e(4);
    if ( echo( 2 + 3 ) != 5 ) e(5);
    if ( echo ( 1 + 2 + 3 + 4 + 5 ) != 10 + echo(5) ) e(6);
    if (( echo( 2<<1 ) ) != 4 ) e(7);
    if ( echo( 2 >> 1 ) != 1 ) e(8);
    if ( echo( 1 << 4 ) != echo( 2 << 3 ) ) e(9);
    if ( echo( echo(4) ) != echo(4) ) e(10);
    if (( echo ( echo ( echo( echo ( echo ( 3 ) ) ) ) ) ) != 3 ) e(11);
    if ( echo( echo( echo(2+3-4+echo(4)*echo(2))) ) != 9 ) e(12);
    if ( min(1,2) != 1) e(13);
    if (min(0,45) != 0) e(14);
    if (min(45,0) != 0) e(15);
    if (min(-72,-100) != -100) e(16);
    if (min(-100,-72) != -100) e(17);
    if (min(1<<3,2<<3) != (1<<3) ) e(18);
    if ( min( echo(3), echo(3) ) != echo (echo(3)) ) e(19);
    if ( max1('a','b') != 'b' ) e(20);
    if ( max1('b','a') != 'b' ) e(21);
    if ( max1(-3,54+2) != ( -3 < 54+2 ? 54+2 : -3 ) ) e(22);
    if (max1('a'+'b'+34,'a'*2) != max2('a'*2,'a'+'b'+34)) e(23);
    if (max1(345/23,4) != max1( echo(345/23), 4) ) e(24);
    if ( max1( max1(2,3), max1(2,3) ) != max1(2,3) ) e(25);
    for (i=3; i<5; i++)
	if ((max1(i,-i)) != i) e(26);
    for (j=min('a',34); j<max2('a',34); j++)
    {
	if ( j<min(max1('a',34),min('a',34)) ) e(27);
	if ( j>max1(min(34,'a'),max2(34,'a')) ) e(28);
    }
    a=b= -32768;
    if ( min(echo(a),a) != a) e(29);
    if ( max1(echo(b),max1(b,b)) != b) e(30);
}




sum(k)
int k;
{
    if (k<=0)
	return(0);
    return(k+sum(k-1));
}




formula(k)
int k;
{
    if (k<=0)
	return(0);
    return ( ((((( (k*(k+1))/2 ))))) );
}




test3()
{
    int k;
    int count;

    t = 3;
    tct++;
    count=0;
    if ( sum(-4) != 0 ) e(1);
    if ( sum(0) != 0 ) e(2);
    if ( sum(2) != 3 ) e(3);
    if ( sum(10) != 55 ) e(4);
    if ( sum(34) != formula(34) ) e(5);
    if ( sum(101) != formula(101) ) e(6);
    if ( sum( sum(11) ) != formula( formula(11) ) ) e(7);
    if ( sum( sum(11) ) != formula( sum(11) ) ) e(8);
    if ( sum( sum( sum(4) )) != sum ( formula ( sum( 4) )) ) e(9);
    for (k = sum(-45); k<sum('a'); k += sum('b') )
	if (count++) e(10);
    if ( echo( sum ( formula( five() ))) != formula ( sum(5) ) ) e(11);
}




test4()
{
    int i,j,k,l,m;
    int a[50];
    int b[1][2][3][4][5];

    t = 4;
    tct++;
    b[0][1][2][3][4] = one();
    if ( b[0][1][2][3][4] != 1) e(1);
    if ( b[0][one()][two()][three()][four()] != 1) e(2);
    if ( b[0][one()][five()-3][one()+2][four()] != b[0][1][2][2*2-1][4]) e(3);
    for (i=0; i<50; i++)
	a[i] = i+1;
    if (a[one()-1] != one()) e(4);
    if (echo(a[4]) != 5) e(5);
    if ( echo( a[ echo(6) ] ) != 7 ) e(6);
    if ( a[a[a[0]]] != 3 ) e(7);
    for (i=11; i<22; i++)
	if ( echo( a[i+echo('b'-'a'-1)] ) != i+1) e(8);
    for (i=0; i<1; i++)
	for (j=0; j<2; j++)
	    for (k=0; k<3; k++)
		for (l=0; l<4; l++)
		    for (m=0; m<5; m++)
			b[echo(i)][echo(j)][echo(k)][l][echo(m)] = j*k*l*m;
    for (i=3; i; --i)
	for (j=4; j; --j)
	    if (b[0][max1( max1(0,0), 1)][2][i][j] != 1*2*echo(i)*j ) e(9);
}




/* More testing */


func(sw,i)
int sw;         /* switch selector */
int i;          /* value to be returned */
{
    int a[10];

    switch(sw)
    {
	case 0: return(-i);
		break;
	case 1: return ( echo(i*i) - (i-1)*i );
		break;
	case 2: return ( 2*func(0,i) );
		break;
	case 3: a[0] = 3;
		a[3] = 1;
		a[1] = 8;
		a[8] = i;
		return( 3*a[a[a[a[0]]]] );
    }
    return(-32768);
}




/* testing function arguments */




f1(i,j)
int i,j;
{
    return( i!=j );
}



f2(ptr)
int *ptr;
{
    int *locptr;

    locptr = ptr;
    return ( *ptr | *locptr );
}



swap(a,b)
int *a,*b;
{
    int temp;

    temp = *a;
    *a = *b;
    *b = temp;
}



test5()
{
    int k,l;
    int *ptr;

    t = 5;
    tct++;
    for (k = -6353; k < -6003; k++)
	if (f1(k,k+3-echo(3))) e(1);
    k = 'a' - 723;
    ptr = &k;
    if ( f2(ptr) != f2(&k) ) e(3);
    if ( f2(ptr) + f2(&k) != 2*k ) e(4);
    if ( f1(*ptr,*ptr) ) e(5);
    if ( f1(*ptr,k) ) e(6);
    if ( f1( f2(&k), *ptr ) ) e(7);
    k = l=28;
    swap(&k,&l);
    if ( k != l ) e(8);
    *ptr = k;
    swap(&k,ptr);
    if ( k != *ptr ) e(9);
    l = -5;
    k = 'h';
    ptr = &k;
    swap(ptr,&l);
    if ( l != 'h' ) e(10);
    if ( k != -5 ) e(11);
    if ( *ptr != -5 ) e(12);
    if ( ptr != &k ) e(13);
}



icount(i)
int i;
{
    return(i+1);
}



test6()
{
    int i;

    tct++;
    t=6;
    for (i=0; i<10; i++)
	if (func(0,i) != -i) e(i);
    for (i=10; i<20; i++)
	if ( -func(2,i) != 2*i) e(i);
    if (func(func(0,0),0) != 0) e(20);
    if (func(-func(0,1),32) != 32) e(21);
    if (max1(32767,10) != max2(32767,10)) e(22);
    if (func(3,10) != 30) e(22);
    if (icount(1)!=2) e(23);
    if (icount(2)!=3) e(24);
    if (icount(32766) != 32767) e(25);
    if (icount((int)-32768) != -32767) e(26);
    if (icount(icount(1)) != 3) e(27);
    if ( icount( (int)&i ) != (int)&i + 1 ) e(28) ;
    if (icount(icount(icount(icount(icount(icount(icount(icount(0))))))))!=8) e(29);
}
