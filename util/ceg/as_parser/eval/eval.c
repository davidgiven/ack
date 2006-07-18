#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "states.h"


#define is_letter( c)	   ( isalpha( c) || isdigit( c) || c == '_')
#define save_or_put(c)	   if(previous_state==TEXT)putchar(c);else *bufptr++=c



/* This program processes the file 'as.c', and will take care of things like
 * %d( <expression>), @func( arg), %$(   ), etc.
 * The main-loop is constructed as a finite automat.
 */


main()
{
	char buffer[256], *bufptr = buffer;
	int c, state = TEXT, previous_state = TEXT, depth = 0;

	pr_header();
	while ( ( c = getchar()) != EOF)
		switch ( state) {
		  case TEXT :
				switch ( c) {
				  case '/' : state = SLASH;
					     previous_state = TEXT;
					     save_or_put( c);
					     break;
				  case '"' : state = STRING;
					     previous_state = TEXT;
					     save_or_put( c);
					     break;
				  case '\'': state = CHAR_CONST;
					     previous_state = TEXT;
					     save_or_put( c);
					     break;
				  case '@' : state = AT;
					     break;
				  default  : putchar( c);
					     break;
			      	}
			      	break;


		  case SLASH :
				state =  ( c == '*' ? COMMENT : TEXT);
			      	save_or_put( c);
			      	break;
		  case COMMENT :
				if ( c == '*')
					state = STAR;
			      	save_or_put( c);
			      	break;
		  case STAR :
				if ( c == '/')
					state = previous_state;	
				else if ( c != '*')
					state = COMMENT;
			      	save_or_put( c);
			      	break;



		  case STRING :
				if ( c == '"')
					state = previous_state;
				else if ( c == '\\')
					state = BACKSLASH_in_STRING;
				save_or_put( c);
				break;

		  case BACKSLASH_in_STRING :
				state = STRING;
				save_or_put( c);
				break;




		  case CHAR_CONST :
				if ( c == '\'')
					state = previous_state;
				else if ( c == '\\')
					state = BACKSLASH_in_CHAR_CONST;
				save_or_put( c);
				break;

		  case BACKSLASH_in_CHAR_CONST :
				state = CHAR_CONST;
				save_or_put( c);
				break;

				
				
		  case AT :	/* @if '(' <CONDITION> ')'
				 * @elsif '(' <CONDITION> ')'
				 * @else
				 * @fi
				 * @<IDENTIFIER> '(' <PARAM_LIST> ')'
				 */
				if ( is_letter( c))
					*bufptr++ = c;
				else {
					ungetc( c, stdin);
					state = WHITE_SPACE;
				}
				break;

		  case WHITE_SPACE :
 				if ( isspace( c))
					*bufptr++ = c;
				else if ( c == '(') {
					*bufptr++ = c;
					depth = 1;
					state = CAL_or_QUEST;
				}
				else {
					*bufptr = '\0';
					pr_ELSE_or_FI( buffer);
					bufptr = buffer;
					ungetc( c, stdin);
					state = TEXT;
				}
				break;

		  case CAL_or_QUEST :	/* match ')' */
				*bufptr++ =c;
				switch ( c) {
				  case '(' : depth++;
					     break;
				  case ')' : depth--;
					     if ( depth == 0) {
						     *bufptr = '\0';
						     pr_CALL_or_QUEST( buffer);
						     bufptr = buffer;
						     state = TEXT;
					     }
					     break;
				  case '/' : state = SLASH;
					     previous_state = CAL_or_QUEST;
					     break;
				  case '"' : state = STRING;
					     previous_state = CAL_or_QUEST;
					     break;
				  case '\'': state = CHAR_CONST;
					     previous_state = CAL_or_QUEST;
					     break;
				}
				break;
		  default :	
				fprintf( stderr, "Unknown state : %d\n", state);
				break;
	}
	exit( 0);
}

pr_header()
{
	printf( "#include \"as_parser.h\"\n");
	printf( "#line 1 \"as.c\"\n");
}


pr_ELSE_or_FI( str)
char *str;
{
	if ( strncmp( str, "else", 4) == 0)
		printf( "fprint( outfile, \"}\else {\");%s", str+4);
	else if ( strncmp( str, "fi", 2) == 0)
		printf( "fprint( outfile, \"}\");%s", str+2);
	else
		fprintf( stderr, "%s  unexpected!!\n", str);
}


pr_CALL_or_QUEST( str)
char *str;
{
	if ( strncmp( str, "if", 2) == 0 && !(is_letter( *(str+2))))
		pr_if( str);
	else if ( strncmp( str, "elsif", 5) == 0 && !(is_letter( *(str+5))))
		pr_elsif( str);
	else
		pr_call( str);
}


/* Adjust 'cur_pos' when necessary */

pr_call( call)
char *call;
{
	char c;

	printf( "{");
	if ( strncmp( "text", call, 4) == 0 && isdigit( *(call+4))) 
		printf( "cur_pos += %d;", *(call+4) - '0');
	else if ( strncmp( "reloc", call, 5) == 0 && isdigit( *(call+5)))
		printf( "cur_pos += %d;", *(call+5) - '0');

	pr_text_with_conversions( call);
	printf( "fprint( outfile, \";\");");
	printf( "}");
	for (; ( c = getchar()) != ';' ; putchar( c));	/* skip ';' */
}

pr_elsif( quest)
char *quest;
{
	printf( "fprint( outfile, \"}\else if\");");
	pr_text_with_conversions( quest+5);
	printf( "fprint( outfile, \"{\");");
}

pr_if( quest)
char *quest;
{
	pr_text_with_conversions( quest);
	printf( "fprint( outfile, \"{\");");
}


pr_text_with_conversions( str)
char *str;
{
	char *ptr, *next_conversion(), *pr_conversion();

        while (  ptr = next_conversion( str)) {
		/* ptr points to '%'-sign */
	 	*ptr = '\0';
		printf( "fprint( outfile, \"");
		pr_string( str);
		printf( "\");");
	 	*ptr = '%';
	        str = pr_conversion( ptr);
	}
	printf( "fprint( outfile, \"");
	pr_string( str);
	printf( "\");");
}


pr_string( s)
char *s;
{
	for ( ; *s != '\0'; s++)
		switch ( *s) {
		  case '"' : printf( "\\\"");
			     break;

		  case '\\': printf( "\\\\");
			     break;

		  case '\n': printf( "\\n");
			     break;

		  default  : printf( "%c", *s);
		}
}


char *next_conversion( str)
char *str;
{
	char *match();

	while ( *str && *str != '%') {
		switch ( *str++) {
		  case '\'' : str = match( '\'', str) + 1;
			      break;
		  case '"'  : str = match( '"', str) + 1;
			      break;
		}
	}
	return( *str == '%' ? str : (char *)0);
}

char *match( c, str)
char c, *str;
{
	while ( *str && ( *str != c || *(str-1) == '\\'))
		str++;
	return( *str ? str : str-1);
}

char *match_bracket( str)
char *str;

/* find ')', but look at nesting '('-')' pairs, return position of ')'.
 */
{
	int depth;
	char *match();

	depth = 1;
	while ( *str && depth != 0) { 
		switch ( *str++) {
		  case '\'' : str = match( '\'', str+1) + 1;
			      break;
		  case '"'  : str = match( '"', str+1) + 1;
			      break;
		  case '('  : depth++;
			      break;
		  case ')'  : depth--;
			      break;
		}
	}
	return( str-1);
}


char *find( c, str)
char c, *str;
{
	while ( *str && *str != c)
		str++;
	return( str);
}
	
char *pr_conversion( str)
char *str;

/* str points to '%'-sign, returns pointer to first character AFTER the
 * conversion
 */
{
	char *start, *ptr, *match_bracket(), *find();

	start = find( '(', str+1);
 	*start++ = '\0';

	ptr = match_bracket( start);
	*ptr = '\0';

	if ( *(str+1) == '$')
		printf( "eval( %s);", start);
	else if ( strncmp( str+1, "dist", 4) == 0)
		printf( "dist( %s);", start);
	else
		printf( "fprint( outfile, \"%%%s\", %s);", str+1, start);

	return( ptr+1);
}
