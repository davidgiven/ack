long _xpipe();

int
_pipe(f)
	int f[2];
{
	long x;

	x = _xpipe();
	if (x == -1) return -1;
	f[0] = x;
	f[1] = x >> 16;
	return 0;
}
