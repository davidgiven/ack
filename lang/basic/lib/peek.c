/* $Id$ */

int peek(addr)
int addr;
{
	/* this can not work properly for machines in which the 
	   POINTERSIZE differs from the integer size
	*/
	char	*p;
	int i;

	p= (char *)addr;
	i= *p;
#ifdef DEBUG
	printf("peek %d = %d\n",addr,i);
#endif
	return(i);
}

_poke(i,j)
int i,j;
{
	char *p;
	p= (char *) i;
	*p=j;
}
