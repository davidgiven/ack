/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>
#include	<signal.h>

extern int _fork(void);
extern int _wait(int *);
extern void _exit(int);
extern void _execl(char *, ...);
extern void _close(int);

#define	FAIL	127

int
system(const char *str)
{
	int pid, exitstatus, waitval;
	int i;

	if ((pid = _fork()) < 0) return str ? -1 : 0;

	if (pid == 0) {
		for (i = 3; i <= 20; i++)
			_close(i);
		if (!str) str = "cd .";		/* just testing for a shell */
		_execl("/bin/sh", "sh", "-c", str, (char *) NULL);
		/* get here if execl fails ... */
		_exit(FAIL);	/* see manual page */
	}
	while ((waitval = _wait(&exitstatus)) != pid) {
		if (waitval == -1) break;
	}
	if (waitval == -1) {
		/* no child ??? or maybe interrupted ??? */
		exitstatus = -1;
	}
	if (!str) {
		if (exitstatus == FAIL << 8)		/* execl() failed */
			exitstatus = 0;
		else exitstatus = 1;			/* /bin/sh exists */
	}
	return exitstatus;
}
