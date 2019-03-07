/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include <stdio.h>
#include "tokentab.h"
#include "system.h"
#include "print.h"

/* Mod van gertjan */
extern int LLsymb;
extern int toknum;


void error_char(char *format,char ch)
{
	extern int listing,errorcnt;
	extern int basicline;

	if ( !listing ) fprint(STDERR, "LINE %d:",basicline);
	fprint(STDERR, format,ch);
	errorcnt++;
}



void error_string(char* format,char* str)
{
	extern int listing,errorcnt;
	extern int basicline;

	if ( !listing ) fprint(STDERR, "LINE %d:",basicline);
	fprint(STDERR, format,str);
	errorcnt++;
}



void LLmessage(int insertedtok )
{
    if ( insertedtok < 0 ) {
	error("Fatal stack overflow\n");
	C_close();
	sys_stop( S_EXIT );
    }

    if ( insertedtok == 0 ) 
	if ( LLsymb < 256 )
	    error_char("%c deleted\n", (char)LLsymb);
	else
	    error_string("%s deleted\n", tokentab[ LLsymb-256 ]);
    else {
	if ( insertedtok < 256 )
	    error_char("%c inserted\n", (char)insertedtok);
	else
	    error_string("%s inserted\n", tokentab[ insertedtok-256 ]);
	toknum = insertedtok;
    }
}
