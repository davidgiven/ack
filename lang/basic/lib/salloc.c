/* $Id$ */

extern char *malloc() ;

char * salloc(length)
unsigned length;
{
	char *c, *s;
	c= malloc(length);
	if( !c ) error(5);
	for(s=c;s<c+length;s++) *s = 0;
	return(c);
}

sfree(c)
char *c;
{
	if( !c ) return;
	free(c);
}
