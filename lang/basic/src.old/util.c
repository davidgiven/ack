#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif

#define abs(X)  (X>=0?X:-X)
/* Miscelaneous routines can be found here */

int	errorcnt;

warning(str)
char *str;
{
	if (! wflag) Xerror("WARNING",str);
}
error(str)
char *str;
{
	Xerror("ERROR",str);
	errorcnt++;
}
Xerror(type,str)
char *str;
char *type;
{
	extern int listing,yylineno;
	if( !listing) fprintf(stderr,"LINE %d:",yylineno);
	fprintf(stderr,"%s:%s\n",type,str);
}
fatal(str)
char *str;
{
	Xerror("FATAL",str);
	exit(-1);
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
	sprintf(buf,"%d",i);
	return(buf);
}
char *instrlabel(i)
int i;
{
	static char buf[30];
	sprintf(buf,"*%d",i);
	return(buf);
}
char *datalabel(i)
int i;
{
	static char buf[30];
	if( i>0)
		sprintf(buf,"l%d",i);
	else	sprintf(buf,"%d",-i);
	return(buf);
}

char *salloc(length)
unsigned length;
{		
	char *s,*c;
	extern char *malloc() ;
	s=c= malloc(length);
	if ( !s ) fatal("Out of memory") ;
	while(length--)*c++ =0;
	return(s);
}

char * proclabel(str)
char *str;
{
	static char buf[50];
	sprintf(buf,"$%s",str);
	return(buf);
}
