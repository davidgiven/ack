long _Xwait();
_wait(p)
	int *p;
{
	long l = _Xwait();

	if (l == -1) return -1;
	if (p) *p = (l >> 16);
	return (int) l;
}
