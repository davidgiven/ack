#include <stdio.h>

/* This file contains the scanner for mylex(), the following functions and 
 * variables are exported :
 *
 *	int yylineno;			- The current line number.
 *
 *	char scanc();			- Return next character.
 *
 *	backc( c);			- Push back one character.
 *		char c;
 *
 *	FILE *switch_input( new);	- Scanner will from now on read from
 *		FILE *new;		- 'new', old input-file is returned.
 *
 * The scanner must perform a lookahead of more than one character, so it uses
 * it's own internal buffer.
 */


int yylineno = 1;


/********* Internal variables + functions ***********************/


#define BUF_SIZE	16

static char buf[BUF_SIZE],	/* Bufer to save backc()-characters */
            *bufptr = buf;	/* Pointer to space for backc()-character */

static FILE *infile = NULL;




static char nextc()
{
	FILE* fp = infile ? infile : stdin;
	
	if ( bufptr > buf)
		return( *--bufptr);
	else
		return( getc( fp));
}


/***************************************************************/


char scanc()

/* Get next character, but delete al C-comments and count lines */

{
	char c, nextc();

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


backc( c)
char c;
{
	if ( bufptr >= buf + BUF_SIZE)
		error( "backc(), no space in buffer left!");
	else {
		if ( c == '\n')
			yylineno--;
		*bufptr++ = c;
	}
}


FILE *switch_input( new)
FILE *new;

/* Switch to a new input file, try to save the lookahead-characters in buf[]
 * by calling ungetc(). If they can't be saved return NULL.
 */

{
	FILE* fp = infile ? infile : stdin;
	char *ptr; FILE *old;

	/* Clean buf[] */
	for ( ptr = buf; ptr < bufptr; ptr++)
		if ( ungetc( *ptr, fp) == EOF && *ptr != EOF)
			return( NULL);

	bufptr = buf;
	old = infile;
	infile = new;
	return( old);
}
