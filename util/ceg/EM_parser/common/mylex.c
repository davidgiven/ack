#include "Lpars.h"
#include <stdio.h>
#include <ctype.h>

/* This file contains the function mylex() which recognizes the following
 * tokens :
 *	EOFILE 		
 *	C_INSTR 	- 'C_loc', 'C_lol', etc.
 *	DEF_C_INSTR 	- 'C_loe..', 'C_ste..', '..icon, '..fcon', etc
 *	CONDITION 	- C-expression, for example: '$1 == 481'
 *	ARROW 		- '==>'
 *	CALL 		- C-style functioncall, for example: 'error( 17)'
 *	ASSEM_INSTR 	- C-style string, for example: '"mov r0, (r1)"'
 *	DEFAULT 	- 'default'
 *	ERROR 		- An error occured in one of the tokens.
 *
 * If the input matches non of these tokens the next character will be returned.
 *
 * Besides mylex() the following variable is exported :
 *
 *	char yytext[];	- Contains the string representation of the current
 *			  token.
 *	char *next;	- Points to the first free position in yytext[].
 */


#define YYTEXT	65536
char yytext[YYTEXT],	/* string-buffer for the token */
     *next;		/* points to the first free posistion in yytext[] */
extern char scanc();

#define FALSE	0
#define TRUE	1

int CD_pos = FALSE;	/* 'CD_pos' is used as a flag to signal if it is
			 * possible to match a CONDITION or DEFAULT-token at 
			 * this moment. Thus mylex() knows about the grammar
			 * of the "EM_table"!!
			 * This flag is needed because CALL is a subset of
			 * CONDITION.
			 */
int CALL_pos = FALSE;	/* Needed to distinguish between 
			 *	C_INSTR CONDITION	and	CALL
			 */


static char skip_space();
static read_string();

int mylex()
{
	char c;

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
		     if ( c != '.') {	/* Just a plain '.', not something like
					 * '..icon'.
					 */
			if ( special)
				CD_pos = TRUE;
			CALL_pos = FALSE;
		     	return( '.');
		     }
		     break;

	  case ';' : return( ';');

	  case '=' : if ( arrow()) {
			CD_pos = FALSE;
			CALL_pos = TRUE;
			return( ARROW);
		     }
		     break;

	  case 'd' : if ( CD_pos && _default()) {
			CD_pos = FALSE;
			special = FALSE;
			return( DEFAULT);
		     }
		     break;
	}
	/* Possible tokens at this place : CONDITION, CALL, C_INSTR,
	 * DEF_C_INSTR
	 */

	if ( CD_pos) {
		read_condition();
		CD_pos = FALSE;
		special = TRUE;
		return( CONDITION);
	}
	if ( isalpha( c)) {
		read_ident();
		if ( CALL_pos) {
			c = skip_space();
			if ( c == '(') {
				*next++ = c;
				read_call();
				return( CALL);
			}
			else {
				backc( c);
				return( ERROR);
			}
		}
		else {
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

static int isletter( c)
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


static read_string()

/* match something like "mov r0, (r1)".
 * strip the double quotes off! Inside a string, the character '"' must
 * be preceded by a '\'.
 */
{
	next--;
	while( ( *next = scanc()) != '"' || *(next-1) == '\\')
		next++;
}

int arrow() /* '==>' */
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

int _default() /* 'default' */
{
	char c;

	if ( ( *next++ = scanc()) == 'e')
	    if ( ( *next++ = scanc()) == 'f')
		if ( ( *next++ = scanc()) == 'a')
		    if ( ( *next++ = scanc()) == 'u')
			if ( ( *next++ = scanc()) == 'l')
			    if ( ( *next++ = scanc()) == 't')
				if ( !isletter( c = skip_space())) {
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

/* A CONDITION is followed by '==>'
 */
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
		}
	}
}

is_C_INSTR( str)
char *str;
{
	if ( *str == 'C' && *(str+1) == '_')	/* C_xxx */
		return( TRUE);
	else
		return( FALSE);
}

is_DEF_C_INSTR( str)
char *str;

/* yytext[] contains either '..[letter]*' ( 2 dots possibly followed by an
 * identifer) * or '[letter]+' ( just an identifier)
 * Try to match something like 'C_loe..' or '..icon'
 */
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
