#include "string.h"
#include "io.h"

/* $Header $ */

/* assume that the channel has been set */

_wrnl()
{
	if( fputc('\n',_chanwr) == EOF) error(29);
}
_wrcomma()
{
	if( fputc(',',_chanwr) == EOF) error(29);
}
_wrint(i)
int i;
{
	if(i>0) 
		if( fputc(' ',_chanwr)==EOF) error(29);
	fprintf(_chanwr,"%d",i);
}
_wrflt(f)
double f;
{
	if( fprintf(_chanwr,"%f",f)== EOF) error(29);
}
_wrstr(s)
String *s;
{
	 if( fprintf(_chanwr,"\"%s\"",s->strval)== EOF) error(29);
}
