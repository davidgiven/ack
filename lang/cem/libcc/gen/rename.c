int
rename(from, to)
	char *from, *to;
{
	(void) unlink(to);
	if (link(from, to) < 0) return -1;
	(void) unlink(from);
	return 0;
}
