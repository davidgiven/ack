#include "bc_string.h"

/* $Id$ */

#define ok(X)	if( X ==0) return;
#define okr(X)	if( X ==0) return(0);

extern char *salloc() ;

_len(str)
String *str;
{
	okr(str);
	return(str->strlength);
}
String *_newstr(str)
char *str;
{
	String *s;
	okr(str);
	s= (String *) salloc(sizeof(String));
	s->strcount=1;
	s->strlength= strlen(str);
	s->strval= salloc(s->strlength+1);
	strcpy(s->strval,str);
	return(s);
}
_incstr(src)
String *src;
{
	/* one more variable uses the string */
	ok(src);
	src->strcount++;
}
_decstr(str)
String *str;
{
	ok(str);
	/* Strings in ROM are initialized with this count */
	if ( str->strcount==9999 ) return ;
	str->strcount--;
	if(str->strcount<=0) _delstr(str);
}
_strcpy(dst,src)
String *src,*dst;
{
	ok(src);
	ok(dst);
	_decstr(dst);
	*dst = *src;
	_incstr(src);
}
_delstr(src)
String *src;
{
	ok(src);
	sfree(src->strval);
	sfree((char *)src);
}
String *_concat(s1,s2)
String *s1,*s2;
{
	String *s;
	int length;
	okr(s1); okr(s2);
	s= (String *) salloc(sizeof(String));
	s->strlength= _len(s1)+_len(s2);
	s->strval= salloc(s->strlength+1);
	s->strcount = 1;
	strcpy(s->strval,s2->strval);
	strcat(s->strval,s1->strval);
	return(s);
}
_strcomp(s1,s2)
String *s1,*s2;
{
	okr(s1);okr(s2);
	return(strcmp(s2->strval,s1->strval));
}

String *_left(size,s)
String *s;
int	size;
{
	String *ns;
	int i;

	okr(s);
	if( size <0 || size >s->strlength) error(3);
	ns= (String *) salloc(sizeof(String));
	ns->strval= salloc(size+1);
	ns->strcount=1;
	for(i=0; i<size && s->strval[i];i++)
		ns->strval[i]= s->strval[i];
	ns->strval[i]=0;
	ns->strlength= i;
	return(ns);
}

String *_space(d)
int d;
{
	String *s;
	int i,len;

	len= d;
	s= (String *) salloc(sizeof(String));
	s->strlength= len;
	s->strcount=1;
	s->strval= salloc(len+1);
	for(i=0;i<len;i++)
		s->strval[i]= ' ';
	s->strval[i]=0;
	return(s);
}

String *_strascii()
{
}
String *_string(f, d)
double	d,f;
{
	int i,j;
	String *s;

	i=d;j=f;
	if( i<0  || i>MAXSTRING) error(3);
	s= (String *) salloc(sizeof(String));
	s->strlength= i;
	s->strcount=1;
	s->strval= salloc(i+1);
	s->strval[i--]=0;
	for(; i>=0;i--)
		s->strval[i]= j;
	return(s);
}
_midstmt(s2,i1,i2,s)
int i1,i2;
String *s, *s2;
{
	int l;

	/*printf("mid called %d %d %s %s\n",i1,i2,s->strval, s2->strval);*/
	if (i2 < 0 || i1 < -1) error(3);
	if( s->strlength<i2 || s2->strlength < i1) error(3);	/* source string too short */
	if( i1== -1) i1= s2->strlength;
	l= s->strlength - i2+1;
	if( i1>l ) i1=l;
	strncpy(s->strval+i2-1,s2->strval,i1);
}
String *_mid(i1,i2,s)
int i1,i2;
String *s;
{
	int l;
	String *s2;

/*	printf("mid fcn called %d %d %s\n",i1,i2,s->strval);*/
	if (i2 < 0 || i1 < -1) return(s2);	/* or error? */
	if( i1 == -1) i1= s->strlength;
	s2= _newstr(s->strval);
	s2->strval[0]=0;
	if( s->strlength<i2) return(s2);	/* source string too short */
	l= s->strlength - i2+1;
	if( i1>l ) i1=l;
	strncpy(s2->strval,s->strval+i2-1,i1);
	s2->strval[i1]=0;
	return(s2);
}

String *_right(length,str)
String *str;
int length;
{
	String *s;
	int i;

	i= _len(str)-length;
	if(i<0) i=0;
	s= _newstr(str->strval+i);
	return(s);
}
