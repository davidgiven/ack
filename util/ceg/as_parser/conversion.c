/* This file contains functions to handle %d, %$, %dist constructs in the
 * as_table.
 */


pr_text_with_conversions( str)
char *str;
{
	char *s, *ptr, *next_conversion(), *pr_conversion();

        while (  ptr = next_conversion( str)) {
		/* ptr points to '%'-sign */
	 	*ptr = '\0';

		out( "fprint( outfile, \"");
		for ( s = str; *s != '\0'; s++)
			if ( *s == '\n')
				out( "\\n");
			else
				out( "%c", *s);
		out( "\");");

	 	*ptr = '%';
	        str = pr_conversion( ptr);
	}

	out( "fprint( outfile, \"");
	for ( s = str; *s != '\0'; s++)
		if ( *s == '\n')
			out( "\\n");
		else
			out( "%c", *s);
	out( "\");");
}

char *next_conversion( str)
char *str;

/* Look for a %-sign, but not in a comment or string! */

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

/* Look for charcter 'c', but watch out for things like \n */

{
	while ( *str && ( *str != c || *(str-1) == '\\'))
		str++;
	return( *str ? str : str-1);
}


char *match_bracket( str)
char *str;

/* find ')', but look at nesting '('-')' pairs, return position of ')'.
 * Skip strings and comments.
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
 * conversion. %$ will result in a call of eval(), %d will call dist().
 */
{
	char *start, *ptr, *match_bracket(), *find();

	start = find( '(', str+1);
 	*start++ = '\0';

	ptr = match_bracket( start);
	*ptr = '\0';

	if ( *(str+1) == '$')
		out( "eval( %s);", start);
	else if ( strncmp( str+1, "dist", 4) == 0)
		out( "dist( %s);", start);
	else
		out( "fprint( outfile, \"%%%s\", %s);", str+1, start);

	return( ptr+1);
}
