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
	printf("WARNING:%s\n",str);
}
error(str)
char *str;
{
	extern int listing,yylineno;
	if( !listing) printf("LINE %d:",yylineno);
	printf("ERROR:%s\n",str);
	errorcnt++;
}
fatal(str)
char *str;
{
	printf("FATAL:%s\n",str);
	exit(-1);
}
notyetimpl()
{
	printf("WARNING: not yet implemented\n");
}
illegalcmd()
{
	printf("WARNING: illegal command\n");
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
int length;
{		
	char *s,*c;
	extern char *malloc() ;
	s=c= malloc(length);
	while(length-->0)*c++ =0;
	return(s);
}

char * proclabel(str)
char *str;
{
	static char buf[50];
	sprintf(buf,"$%s",str);
	return(buf);
}
