long _wait();

wait(p)
	int *p;
{
	long l = _wait();

	if (l == -1) return -1;
	if (*p) *p = (l >> 16);
	return (int) l;
}
