#

/* $Header$ */

/* Strings are allocated in a fixed string descriptor table 
** This mechanism is used to avoid string copying as much as possible
*/

typedef struct{
	char	*strval;
	int	strcount;
	int	strlength;
	} String;

String *_newstr() ;

#define MAXSTRING 1024
