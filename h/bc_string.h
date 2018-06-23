/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#

/* $Id$ */

/* Strings are allocated in a fixed string descriptor table 
** This mechanism is used to avoid string copying as much as possible
*/

typedef struct{
	char	*strval;
	int	strcount;
	int	strlength;
	} String;

extern String *_newstr(char* str);

#define MAXSTRING 1024
