/* $Id$ */

_sgn(v)
double v;
{
	if( v>0) return(1);
	if( v<0) return(-1);
	return(0);
}

_forsgn(v)
double v;
{
	if (v >= 0) return 1;
	return -1;
}
