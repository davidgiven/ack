long _time();

long
time(p)
	long *p;
{
	long l = _time();
	if (p) *p = l;

	return l;
}
