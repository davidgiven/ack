/* $Id$ */

/*
	Test combination of signal and setjmp/longjmp
*/

#include <setjmp.h>
#include <signal.h>

jmp_buf jb;
void a(), b(); 

main()
{
	char c;

        signal( SIGINT, a );
        signal( SIGQUIT, b );
	switch( setjmp( jb ) ) {
	    case 0: write( 1, "start\n", 6 ); break;
	    case 1: write( 1, "int\n", 4 ); break;
	    case 2: write( 1, "quit\n", 5 ); break;
	}
	while(read(0, &c, 1));
	
}

void a(){ signal( SIGINT, a ); longjmp( jb, 1 ); }
void b(){ signal( SIGQUIT, b ); longjmp( jb, 2 ); }
