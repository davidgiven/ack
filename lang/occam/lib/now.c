/* $Id$ */
long now()
{
	extern int deadlock;
	long time();

	deadlock = 0;
	return time((long *) 0);
}
