#include <stdio.h>

FILE *infile = stdin;

#define BUF_SIZE	16
char buf[BUF_SIZE],	/* Bufer to save backc()-characters */
     *bufptr = buf;	/* Pointer to space for backc()-character */
int yylineno = 1;


char nextc()
{
	if ( bufptr > buf)
		return( *--bufptr);
	else
		return( getc( infile));
}


backc( c)
char c;
{
	if ( bufptr > buf + BUF_SIZE)
		error( "backc(), no space in buffer left!");
	else {
		if ( c == '\n')
			yylineno--;
		*bufptr++ = c;
	}
}


char scanc()

/* Get next character, but delete al C-comments */

{
	char c;

	c = nextc();
	while ( c == '/') {
		c = nextc();
		if ( c == '*') {	/* start of comment */
			while ( nextc() != '*' || nextc() != '/')
				;
			c = nextc();
		}
		else {
			backc( c);
			return( '/');
		}
	}
	if ( c == '\n')
		yylineno++;
	return( c);
}


FILE *switch_input( new)
FILE *new;

/* Return the current FILE, if buf[] can't be cleaned NULL will be returned */

{
	char *ptr; FILE *old;

	/* Clean buf[] */
	for ( ptr = buf; ptr < bufptr; ptr++)
		if ( ungetc( *ptr, infile) == EOF && *ptr != EOF)
			return( NULL);

	bufptr = buf;
	old = infile;
	infile = new;
	return( old);
}
