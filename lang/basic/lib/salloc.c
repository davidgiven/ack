/* $Header $ */

char * salloc(length)
int length;
{
	char *c, *s;
	c= (char *) malloc(length);
	if( c== (char *) -1) error(5);
	for(s=c;s<c+length;s++) *s = 0;
	return(c);
}

sfree(c)
char *c;
{
	if( c== 0) return;
	free(c);
}
