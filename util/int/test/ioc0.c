/* $Id$ */

/* Testing ioctl monitor call */
#include	<sgtty.h>

char sbuf[10];
struct sgttyb old, ttyb;

main()
{
   register i = 0;
   char c;

   if( ioctl( 1, TIOCGETP, &old ) != 0 ) {
       write( 2, "ioctl ophalen mislukt\n", 22 );
       exit( 100 );
   }
   write( 2, "Huidige status opgehaald\n", 25 );
   ttyb = old;
   ttyb.sg_flags &= ~ECHO;
   if( ioctl( 1, TIOCSETP, &ttyb ) != 0 ) {
       write( 2, "ioctl -echo mislukt\n", 20 );
       exit( 100 );
   }
   write( 2, "Echo uitgezet\n", 14 );
   write( 2, "geef input: ", 12 ); 
   while( i<9 && (c = getchar()) != '\n' )
       sbuf[i++] = c;
   write( 1, sbuf, strlen(sbuf) );
   if( ioctl( 1, TIOCSETP, &old ) != 0 ) {
       write( 2, "ioctl reset mislukt\n", 20 );
       exit( 100 );
   }
   write( 2, "Klaar\n", 6 );
   exit( 0 );
}
