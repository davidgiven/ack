nice(incr)
{
	extern int errno;
	int sav = errno;
	int prio;

	errno = 0;
	prio = getpriority(0,0);
	if (prio == -1 && errno) return -1;
	if (setpriority(0,0,prio+incr) < 0) return -1;
	errno = sav;
	return 0;
}
