long _xtime();

long
time(p)
	long *p;
{
	long l = _xtime();
	if (p) *p = l;

	return l;
}
