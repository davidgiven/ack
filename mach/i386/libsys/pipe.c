long _pipe();

int
pipe(f)
	int f[2];
{
	long x;

	x = _pipe();
	if (x == -1) return -1;
	f[0] = x;
	f[1] = x >> 16;
	return 0;
}
