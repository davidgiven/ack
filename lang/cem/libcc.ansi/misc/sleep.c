/*
 * sleep - suspend current process for a number of seconds
 */
/* $Header$ */

#include	<signal.h>
#include	<setjmp.h>

int alarm(int n);
void pause(void);

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
	unsigned oldalarm;
	void (*oldsig)(int);

	if (n <= 0) return;
	if (setjmp(setjmpbuf)) {
		signal(SIGALRM, oldsig);
		alarm(oldalarm);
		return;
	}
	oldalarm = alarm(5000);		/* Who cares how long, as long
					 * as it is long enough
					 */
	if (oldalarm > n) oldalarm -= n;
	else if (oldalarm) {
		n = oldalarm;
		oldalarm = 1;
	}
	oldsig = signal(SIGALRM, alfun);
	alarm(n);
	for (;;)  {
		/* allow for other handlers ... */
		pause();
	}
}
