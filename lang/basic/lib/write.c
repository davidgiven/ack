#include "bc_string.h"
#include "bc_io.h"

/* $Id$ */

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
	if( ferror(_chanwr) ) error(29);
}
_wrflt(f)
double f;
{
	fprintf(_chanwr,"%f",f);
	if( ferror(_chanwr) ) error(29);
}
_wrstr(s)
String *s;
{
	fprintf(_chanwr,"\"%s\"",s->strval);
	if( ferror(_chanwr) ) error(29);
}
