/* $Id$ */

#include <signal.h>

/* testing signal(SIGINT, vang) */

int handler();

main()
{
	char *ch = "a\n";
	
	signal( SIGINT, handler );
	while(1) {
		write(1, ch, 2);
		ch[0]++;
		if (ch[0] > 'z')
			ch[0] = 'a';
	}
}

handler()
{
	/* vang CTRL-C op */
	write( 1, "Heb um gevangen\n", 16 );
	signal( SIGINT, handler );
}
