#include "string.h"

/* $Header $ */

String *_mki(i)
int i;
{
	char *buffer ="  ";
	String *s;

	s= (String *) _newstr(buffer);
	strncpy(s->strval,&i,2);
	return(s);
}
String *_mkd(d)
double d;
{
	char *buffer ="        ";
	String *s;

	s= (String *) _newstr(buffer);
	strncpy(s->strval,&d,8);
	return(s);
}
_cvi(s)
String *s;
{
	int i;
	strncpy(&i,s->strval,2);
	return(i);
}
double _cvd(s)
String *s;
{
	double d;
	strncpy(&d,s->strval,8);
}
