/* $Header$ */
#include <signal.h>

system(str)
	char *str;
{
	int pid, exitstatus, waitval;
	void (*sigint)(), (*sigquit)();
	int i;

	if ((pid = fork()) < 0) return -1;	/* How do we distinguish this
						   from exit status -1?
						*/
	if (pid == 0) {
		for (i = 3; i <= 20; i++) close(i);
		execl("/bin/sh", "sh", "-c", str, (char *) 0);
		/* get here if execl fails ... */
		_exit(127);	/* see manual page */
	}

	sigint  = signal( SIGINT,  SIG_IGN );
	sigquit = signal( SIGQUIT, SIG_IGN );

	while ((waitval = wait(&exitstatus)) != pid) {
		if (waitval == -1) break;
	}
	if (waitval == -1) {
		/* no child ??? or maybe interrupted ??? */
		exitstatus = -1;
	}

	signal( SIGINT,  sigint );
	signal( SIGQUIT, sigquit );

	return exitstatus;
}
