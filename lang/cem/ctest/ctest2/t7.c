#
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

char rcs_id[] = "$Id$" ;


/*
#define TEST1 1
*/


/* This program can be used to test C-compilers */
/* It is supposed the first test program (= "test1") */
/* is used to test the basic arithmetic */

/* The following are global counters */

int t,		/* the value indicates the number of the test procedure */
    ect,	/* error counter					*/
    tct;	/* count the number of test procedures called		*/

/************************************************************************/
/*									*/
/* The following is tested:						*/
/* FOR STATEMENTS in test1						*/
/* WHILE STATEMENTS in test2						*/
/* WHILE and FOR STATEMENTS in test3					*/
/* DO STATEMENTS in test4						*/
/* SWITCH STATEMENTS in test5						*/
/*									*/
/************************************************************************/



char *pp1 = "End of test program, ";
char *pp2 = " test(s) completed, ";
char *pp3 = " errors detected\n";
char *pp4 = "Error ";
char *pp5 = " in test";
char *pp6 = "\n";

itoa(p,ptr)
/* converts integer "p" to ascii string "ptr" */
int p;
char *ptr;
{
    register int k,l;
    register char *str;
    int sign;

    str=ptr;
    k=p;
    if ((sign=k)<0)
	k = -k;
    do
    {
	l = k % 10;
	k /= 10;
        *str++ = l + '0';
    }
    while(k);
    if (sign<0)
	*str++ = '-';
    *str = '\0';
    reverse(ptr);
}



reverse(s)
char s[];
{
    register int c,i,j;

    for (i=0, j=strlen(s)-1; i<j; i++, j--)
    {
	c=s[i];
	s[i]=s[j];
	s[j]=c;
    }
}

strlen(str)
/* returns the length of string str */
char *str;
{
    register char *s, *p;

    p = s = str;
    while (*p)
	p++;
    return(p-s);
}



main()
{
    char chtct[10],chect[10];
    tct = 0;
    ect = 0;		/* No errors, so far so good			*/
    test1();		/* testing FOR STATEMENTS			*/
    test2();		/* testing WHILE STATEMENTS			*/
    test3();		/* testing combined FOR and WHILE statements	*/
    test4();		/* testing DO statements			*/
    test5();		/* testing SWITCH statements			*/
    test6();		/* testing GOTO statements			*/
    test7();
    test8();
    write(1,pp1,strlen(pp1));
    itoa(tct,chtct);
    write(1,chtct,strlen(chtct));
    write(1,pp2,strlen(pp2));
    itoa(ect,chect);
    write(1,chect,strlen(chect));
    write(1,pp3,strlen(pp3));
    return(ect);
}



e(n)			/* prints an error message			*/
int n;
{
    char cht[10],chn[10];
    ect++;		/* total number of errors increased by 1	*/
    write(1,pp4,strlen(pp4));
    itoa(n,chn);
    write(1,chn,strlen(chn));
    write(1,pp5,strlen(pp5));
    itoa(t,cht);
    write(1,cht,strlen(cht));
    write(1,pp6,strlen(pp6));
}



test1()		/* Testing the for statement */
{
    int i, j;	/* variables, used as contolling integers in the	*/
		/* for statements					*/

    t = 1;		/* This is test 1 				*/
    tct++;
    for ( ; ; )
    {
	break;
	e(1);
	return;		/* If the break doesn't work, let's hope the	*/
			/* return does ! 				*/
    }
    for ( ; ; )
    {
	for ( ; ; )
	{
	    for ( ; ; )
	    {
		for ( ; ; )
		{
		    for ( ; ; )
		    {
			for ( ; ; )
			{
			    break;
			    e(2);
			    return;
			}
			break;
			e(3);
			return;
		    }
		    break;
		    e(4);
		    return;
		}
		break;
		e(5);
		return;
	    }
	    break;
	    e(6);
	    return;
	}
	break;
	e(7);
	return;
    }
    i=0;
    for ( ; ; i++)
    {
	break;
	e(8);
	return;
    }
    for (i=0 ; ; i++)
    {
	break;
	e(9);
	return;
    }
    for (i=0 ; i<3; i++)
	;
    if (i != 3) e(10);
    for (i=0; i<0; i++)
	e(11);
    if (i != 0) e(12);
    for (i=0; i<1; i++)
	for (i=i; i<i; i++)
	    for (i=i; i<(i+0); i++)
		for (i=i+0; i<(i-0); i++)
		    for (i=i-0; i<i; i++)
			e(13);
    if (i != 1) e(14);
    for (i=0; i<3; )
	i++;
    if (i != 3) e(15);
    i = 18;
    j = 3;
    for ( ; j<i; --i)
	;
    if (i != j) e(16);
    if (i != 3) e(17);
    j = -30;
    for ( ; ; )
	if (++j)
	    continue;
	else break;
    if (j != 0) e(18);
    i = 0;
    for (i++, i++, i++, i++; ; )
    {
	if (i != 4) e(19);
	break;
    }
    i = 1;
    for (i=j=i=j=i=j=i; i && j && i; --i, --j)
    {
	if (i != 1) e(20);
    }
    j=0;
    for (i=32700; i<32767; i++)
	j++;
    if (j != 67) e(21);
    j=0;
#ifdef TEST1
    printf("*** 1\n");
    for (i=32000; i<=32767; i++)
	j++;
    if (j != 68) e(22);
    printf("*** 2\n");
#endif
    j=0;
    for (i=32767; i>32700; i--)
	j++;
    if (j != 67) e(23);
    j=0;
    for (i= -32768; i<-32700; i++)
	j++;
    if (j != 68) e(24);
}




test2()		/* Testing the while statement */
{
    int i, j;

    t = 2;
    tct++;
    while(1)
    {
	break;
	e(1);
	return;
    }
    while(0)
    {
	e(2);
	break;
	e(3);
	return;
    }
    while (1 || 0)
    {
	break;
	e(4);
	return;
    }
    while (1 && 0)
    {
	e(5);
	break;
	e(6);
	return;
    }
    j = 10;
    while (--j)
	;
    if (j != 0) e(7);
    while (j)
    {
	e(8);
	break;
    }
    while ( i=j )
    {
	e(9);
	break;
    }
    while ( (i==j) && (i!=j) )
    {
	e(10);
	break;
    }
    j = 1;
    while (j)
	while(j)
	    while(j)
		while(j)
		    while(j)
			while(--j)
			    ;
    if (j != 0) e(11);
    if (j) e(12);
    j = 30;
    while (--j)
    {
	continue;
	continue;
	continue;
	continue;
	continue;
	break;
	e(13);
    }
}




test3()		/* Combined FOR and WHILE statements */
{
    int i,j;

    t = 3;
    tct++;
    j = 0;
    for (i=3; i; i++)
    {
	while (i--)
	    ;
	if (++j > 1) e(1);
    }
}




test4()		/* Do statement */
{
    int i;

    t = 4;
    tct++;
    i = 0;
    do
	if (i) e(1);
    while (i);
    do
    {
	do
	{
	    do
	    {
		do
		{
		    i++;
		}
		while (!i);
		i++;
	    }
	    while (!i);
	    i++;
	}
	while (!i);
	i++;
    }
    while (!i);
    if (i != 4) e(2);
}




test5()		/* SWITCH statement */
{
    int i,j;

    t = 5;
    tct++;
    for (i=0; i<10; i++)
    {
	switch (i)
	{
	    case 0: if (i != 0) e(1);
		    break;
	    case 1: if (i != 1) e(2);
		    break;
	    case 2: if (i != 2) e(3);
		    break;
	    case 3: if (i != 3) e(4);
		    i++;
	    case 4: if (i != 4) e(5);
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
		    break;
	    default: e(6);
	}
    }
    for (i=j= -18; i<10; i++, j++)
    {
	switch (i)
	{
	    case -3:
	    case 7:
	    case 1: switch (j)
		    {
			case -3:
			case 7:
			case 1:
				break;
			default: e(7);
		    }
		    break;
		    e(8);
	    case -4: switch (j)
		     {
			case -4: if (i != -4) e(9);
				 break;
			default: e(10);
		     }
	}
    }
    i = 'a';
    switch (i)
    {
	case 'a':
	    switch ( i )
	    {
		case 'a':
		    switch ( i )
		    {
			case 'a':
			    break;
			default: e(11);
		    }
		    break;
		default: e(12);
	    }
	    break;
	default: e(13);
    }
}



test6()		/* goto statement */
{
    int k;

    t = 6;
    tct++;
    k = 0;
    goto lab0;
xl1:
    k = 1;
    goto lab1;
xl2:
    k = 2;
    goto lab2;
xl3:
    k = 3;
    goto lab3;
xl4:
    k = 4;
    goto llab1;
llab2: goto llab3;
llab4: goto llab5;
llab6: goto llab7;
llab8: if ( k != 4 ) e(5);
	return ;
llab1: goto llab2;
llab3: goto llab4;
llab5: goto llab6;
llab7: goto llab8;
lab0: if ( k!= 0 ) e(1);
    goto xl1 ;
lab1: if ( k!= 1 ) e(2);
    goto xl2 ;
lab2: if ( k!= 2 ) e(3);
    goto xl3 ;
lab3: if ( k!= 3 ) e(4);
    goto xl4 ;
}



test7()		/* Combinations of FOR, WHILE, DO and SWITCH statements */
{
    int i,j,k;

    t = 7;
    tct++;
    for ( i=j=k=0; i<6; i++, j++, k++ )
    {
	if ( i != j ) e(1);
	if ( i != k ) e(2);
	if ( j != k ) e(3);
	while ( i > j )
	{
	    e(4);
	    break;
	}
	while ( i > k )
	{
	    e(5);
	    break;
	}
	while ( j != k )
	{
	    e(6);
	    break;
	}
	switch(i)
	{
	    case 0:
		switch(j)
		{
		    case 0:
			switch(k)
			{
			    case 0: if ( i+j+k != 0 ) e(7);
				    break;
				    e(8);
			    default: if ( i+j+k != k ) e(9);
			}
			break;
		    default: if ( j > 6 ) e(10);
			     if ( k != j ) e(11);
		}
		break;
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5: break;
	    default: e(12);
	}
    }
    for ( i=j= -3; i<0; i++,j++)
	if ( j == -3 )
	    do
		if ( i )
		    switch ( i )
		    {
			case -3: if ( j != i ) e(13);
		        case -2: if ( j != i ) e(14);
			case -1: for ( k=i; k < 2*j-j; k++)
				 {
				     e(15);
				     break;
				 }
				 break;
			case 0: e(16);
				break;
			default: e(17);
				 break;
		    }
		else e(18);
	    while ( 0 );
    if ( i != j ) e(19);
}




test8()
{
    int *p1, *p2;
    int i,j,k;
    int a1[1], a2[2][2], a3[3][3][3];

    t = 8;
    tct++;
    a1[0] = 0;
    for ( i=0; i<2; i++ )
	for ( j=0; j<2; j++ )
	    a2[i][j] = (i*j) ^ (i+j);
    if ( a2[0][0] != 0 ) e(1);
    if ( a2[0][1] != 1 ) e(2);
    if ( a2[1][0] != a2[0][1] ) e(3);
    for ( i=0; i<3; i++)
	for (j=0; j<3; j++)
	    for (k=0; k<3; k++)
		a3[i][j][k] = i | j | k;
    if ( a3[0][0][0] != 0 ) e(4);
    if ( a3[0][1][2] != a3[2][0][1] ) e(5);
    if ( a3[2][1][1] != (2 | 1 | 1) ) e(6);
    p2 = &a3[0][1][2];
    p1 = &a3[0][1][2];
    for ( ; p1 == p2 ; p1++ )
    {
	switch ( *p1 )
	{
	    case 3: break;
	    default: e(7);
	}
	if ( *p1 != *p2 ) e(8);
    }
}
