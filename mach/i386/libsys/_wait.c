long _xwait();

_wait(p)
	int *p;
{
	long l = _xwait();

	if (l == -1) return -1;
	if (p) *p = (l >> 16);
	return l & 0xffff;
}
