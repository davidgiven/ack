/* $Id$ */

/*
	Test forking
*/

#include <signal.h>

main()
{
    int i, b;
    long a;

    i = fork();
    if( i == 0 ) {
	printf( "kind 1\n" );
	i = fork();
	if( i == 0 ) {
	    printf( "kind 1.1\n" );
	    exit( 111 );
	} else {
	    wait( &b );
	    printf( "h:%d, l:%d\n", (b&0xFF00)>>8, b&0xFF);
	    exit( 11 );
	}
    } else {
	printf( "parent\n" );
	i = fork();
	if( i == 0 ) {
	    printf( "kind 2\n" );
	    exit( 22 );
	} else {
	    a = wait( &b );
	    printf( "pid:%d, h:%d, l:%d\n", a, (b&0xFF00)>>8, b&0xFF);
	    a = wait( &b );
	    printf( "pid:%d, h:%d, l:%d\n", a, (b&0xFF00)>>8, b&0xFF);
	    exit( 99 );
	}
    }
}
