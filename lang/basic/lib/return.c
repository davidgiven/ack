/* $Id$ */

#define MAXNESTING	1000

int _gotable[MAXNESTING];
int topstk=0;

_gosub(x)
int x;
{
	/* administer gosub */
#ifdef DEBUG
	printf("store %d in %d\n",x,topstk);
#endif
	if( topstk== MAXNESTING)	error(26);
	_gotable[topstk]= x;
	topstk++;
}
_retstmt()
{
	/* make sure that a return label index is on top
	  of the stack */
#ifdef DEBUG
	printf("return to %d %d\n",_gotable[topstk-1],topstk-1);
#endif
	  if( topstk==0 || topstk==MAXNESTING) 
		error(1);
	  return( _gotable[--topstk]);
}
