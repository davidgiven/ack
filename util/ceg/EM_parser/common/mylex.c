#include "Lpars.h"
#include <stdio.h>
#include <ctype.h>

#define YYTEXT	256
#define FALSE	0
#define TRUE	1

char yytext[YYTEXT], *next;
int CD_pos = FALSE;	/* condition or default allowed ? */

int mylex()
{
	char c, scanc(), skip_space();

	static int special = FALSE;	/* rule with conditions + default ? */

	next = yytext;
	c = *next++ = skip_space();
	switch ( c) {
	  case EOF : next = yytext;
		     return( 0);

	  case '"' : read_string();
		     return( ASSEM_INSTR);

	  case '.' : c = scanc();
		     backc( c);
		     if ( c != '.') {	/* geen ..icon o.i.d. */
			if ( special)
				CD_pos = TRUE;
		     	return( '.');
		     }
		     break;

	  case ';' : return( ';');

	  case '=' : if ( arrow()) {
			CD_pos = FALSE;
			return( ARROW);
		     }
		     break;

	  case ':' : if ( equiv()) {
			CD_pos = FALSE;
			return( EQUIV);
		     }
		     break;

	  case 'd' : if ( CD_pos && _default()) {
			CD_pos = FALSE;
			special = FALSE;
			return( DEFAULT);
		     }
		     break;
	}
	if ( CD_pos) {
		read_condition();
		CD_pos = FALSE;
		special = TRUE;
		return( CONDITION);
	}
	if ( isalpha( c)) {
		read_ident();
		c = skip_space();
		if ( c == '(') {
			*next++ = c;
			read_call();
			return( CALL);
		}
		else {
			backc( c);
			if ( is_DEF_C_INSTR( yytext)) {
				CD_pos = TRUE;
				return( DEF_C_INSTR);
			}
			if ( is_C_INSTR( yytext)) {
				CD_pos = TRUE;
				return( C_INSTR);
			}
			return( ERROR);
		}
	}
	if ( c == '.') {
		c = scanc();
		if ( c == '.') {
			*next++ = '.';
			read_ident();
			if ( is_DEF_C_INSTR( yytext)) {
				CD_pos = TRUE;
				return( DEF_C_INSTR);
			}
			return( ERROR);
		}
		else {
			backc( c);
			return( '.');
		}
	}
	return( c);
}

int isletter( c)
char c;
{
	return( isalpha( c) || isdigit( c) || c == '_');
}

static char skip_space()
{
	char c;

	while ( isspace( c = scanc()))
		;
	return( c);
}


/* first character has been read */

read_string()

/* de "'s eraf strippen!! */

{
	next--;
	while( ( *next = scanc()) != '"' || *(next-1) == '\\')
		next++;
}

int arrow()
{
	if ( ( *next++ = scanc()) == '=')
		if ( ( *next++ = scanc()) == '>')
			return( TRUE);
		else
			backc( *--next);
	else
		backc( *--next);
	return( FALSE);
}

int equiv()
{
	if ( ( *next++ = scanc()) == ':')
		if ( ( *next++ = scanc()) == '=')
			return( TRUE);
		else
			backc( *--next);
	else
		backc( *--next);
	return( FALSE);
}

int _default()
{
	char c;

	if ( ( *next++ = scanc()) == 'e')
	    if ( ( *next++ = scanc()) == 'f')
		if ( ( *next++ = scanc()) == 'a')
		    if ( ( *next++ = scanc()) == 'u')
			if ( ( *next++ = scanc()) == 'l')
			    if ( ( *next++ = scanc()) == 't')
				if ( !isletter( c = scanc())) {
					backc( c);
					return( TRUE);
				}
				else
					backc( c);
			    else
				backc( *--next);
			else
			    backc( *--next);
		    else
			backc( *--next);
		else
		    backc( *--next);
	    else
		backc( *--next);
	else
	    backc( *--next);
	return( FALSE);
}

read_ident()
{
	char c;

	while ( isletter( c = scanc()))
		*next++ = c;
	backc( c);
}

read_call()
{
	int n = 1;

	while ( TRUE)
		switch( *next++ = scanc()) {
		  case EOF : return;

		  case '(' : n++;
			     break;

		  case ')' : n--;
			     if ( n == 0)
				return;
			     break;
		}
}

read_condition()
{
	while ( TRUE) {
		switch ( *next++ = scanc()) {
		  case EOF : return;

		  case '=' : if ( arrow()) {
				backc( '>');
				backc( '=');
				backc( '=');
				next -= 3;
				return;
			     }
			     break;

		  case ':' : if ( equiv()) {
				backc( '=');
				backc( ':');
				backc( ':');
				next -= 3;
				return;
			     }
			     break;
		}
	}
}

is_C_INSTR( str)
char *str;
{
	if ( *str == 'C' && *(str+1) == '_')
		return( TRUE);
	else if ( strncmp( "locals", str, 6) == 0)
		return( TRUE);
	else if ( strncmp( "jump", str, 4) == 0)
		return( TRUE);
	else if ( strncmp( "prolog", str, 6) == 0)
		return( TRUE);
	else
		return( FALSE);
}

is_DEF_C_INSTR( str)
char *str;

/* Er is gelezen [..][letter]* */

{
	if ( *str == '.' && *(str+1) == '.')
		return( next > yytext+1);

	if ( ( *next++ = scanc()) == '.')
		if ( ( *next++ = scanc()) == '.')
			return( next > yytext+1);
		else
			backc( *--next);
	else
		backc( *--next);
	return( FALSE);
}
