#include "bc_string.h"

/* $Id$ */

String *_mki(i)
long i;
{
	char *buffer ="    ";
	String *s;

	s= _newstr(buffer);
	* ( (long *)s->strval ) = i ;
	return(s);
}
String *_mkd(d)
double d;
{
	char *buffer ="        ";
	String *s;

	s= _newstr(buffer);
	* ( (double *)s->strval ) = d ;
	return(s);
}
long _cvi(s)
String *s;
{
	return *( (long *) s->strval) ;
}
double _cvd(s)
String *s;
{
	return *( (double *) s->strval) ;
}
