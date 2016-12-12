/* $Id$ */

int _sgn(double v)
{
	if( v>0) return(1);
	if( v<0) return(-1);
	return(0);
}

int _forsgn(double v)
{
	if (v >= 0) return 1;
	return -1;
}
