long
time(loc)
	long *loc;
{
	struct { long l1,l2; } t1;

	if (gettimeofday(&t1, (char *) 0) < 0) {
		return -1;
	}
	if (loc) *loc = t1.l1;
	return t1.l1;
}
