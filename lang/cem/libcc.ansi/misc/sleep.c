/*
 * sleep - suspend current process for a number of seconds
 */
/* $Id$ */

#include	<signal.h>
#include	<setjmp.h>

int _alarm(int n);
void _pause(void);

static jmp_buf	setjmpbuf;

static void
alfun(int sig)
{
	longjmp(setjmpbuf, 1);
}		/* used with sleep() below */

void
sleep(int n)
{
/* sleep(n) pauses for 'n' seconds by scheduling an alarm interrupt. */
	unsigned oldalarm = 0;
	void (*oldsig)(int) = 0;

	if (n <= 0) return;
	if (setjmp(setjmpbuf)) {
		signal(SIGALRM, oldsig);
		_alarm(oldalarm);
		return;
	}
	oldalarm = _alarm(5000);	/* Who cares how long, as long
					 * as it is long enough
					 */
	if (oldalarm > n) oldalarm -= n;
	else if (oldalarm) {
		n = oldalarm;
		oldalarm = 1;
	}
	oldsig = signal(SIGALRM, alfun);
	_alarm(n);
	for (;;)  {
		/* allow for other handlers ... */
		_pause();
	}
}
