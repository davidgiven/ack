/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "system.h"
#include "util.h"
#include "bem.h"


#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

#define abs(X)  (X>=0?X:-X)
/* Miscelaneous routines can be found here */

int	errorcnt;




static void Xerror(char *type, char *str)
{
	extern int listing;
	extern int basicline;

	if( !listing) fprint(STDERR, "LINE %d:",basicline);
	fprint(STDERR, "%s:%s\n",type, str);
}




void warning(char* str)
{
	 if (wflag) return;
	 Xerror("WARNING", str);
}


void error(char* str)
{
	Xerror("ERROR", str);
	errorcnt++;
}



void fatal(char* str)
{
	Xerror("FATAL",str);
	C_close();
	sys_stop(S_EXIT);
}



void notyetimpl(void)
{
	warning("not yet implemented");
}



void illegalcmd(void)
{
	warning("illegal command");
}



char *myitoa(int i)
{
	static char buf[30];

	sprint(buf,"%d",i);
	return(buf);
}


char *salloc(unsigned int length)
{		
	char *s,*c;

	s=c=malloc(length);
	if ( !s ) fatal("Out of memory") ;
	while(length--)*c++ =0;
	return(s);
}



