/* $Id$ */

#if !defined(EM_WSIZE)
#define EM_WSIZE _EM_WSIZE
#endif

_randomi()
{
	int i;
	_setchan(-1);
	printf("Random number seed (-32768 to 32767) ? ");
	_readint(&i);
	_setrand(i);
}

_setrand(i)
	int i;
{
	srand(i);
}
double _rnd(d) double d;
{
	double f; f= (int) rand();
	return(f/
#if EM_WSIZE == 4
		2147483647.0
#else
		32767.0
#endif
	);
}
