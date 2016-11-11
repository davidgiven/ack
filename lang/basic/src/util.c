/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Id$" ;
#endif

#define abs(X)  (X>=0?X:-X)
/* Miscelaneous routines can be found here */

int	errorcnt;



void
warning(str)
char *str;
{
	 if (wflag) return;
	 Xerror("WARNING", str);
}


error(str)
char *str;
{
	Xerror("ERROR", str);
	errorcnt++;
}

Xerror(type, str)
char *str;
char *type;
{
	extern int listing;
	extern int basicline;

	if( !listing) fprint(STDERR, "LINE %d:",basicline);
	fprint(STDERR, "%s:%s\n",type, str);
}



fatal(str)
char *str;
{
	Xerror("FATAL",str);
	C_close();
	sys_stop(S_EXIT);
}



notyetimpl()
{
	warning("not yet implemented");
}



illegalcmd()
{
	warning("illegal command");
}



char *itoa(i)
int i;
{
	static char buf[30];

	sprint(buf,"%d",i);
	return(buf);
}






char *salloc(length)
unsigned length;
{		
	char *s,*c;

	s=c=malloc(length);
	if ( !s ) fatal("Out of memory") ;
	while(length--)*c++ =0;
	return(s);
}



