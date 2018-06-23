/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#if ACKCONF_WANT_EMULATED_SYSTEM

#define FAIL 127

static const char* exec_tab[] = {
	"sh", /* argv[0] */
	"-c", /* argument to the shell */
	NULL, /* to be filled with user command */
	NULL /* terminating NULL */
};

int system(const char* str)
{
	int pid, exitstatus, waitval;
	int i;

	if ((pid = fork()) < 0)
		return str ? -1 : 0;

	if (pid == 0)
	{
		for (i = 3; i <= 20; i++)
			close(i);
		if (!str)
			str = "cd ."; /* just testing for a shell */
		exec_tab[2] = str; /* fill in command */
		execve("/bin/sh", (char* const*)exec_tab, (char* const*)environ);
		/* get here if execve fails ... */
		_exit(FAIL); /* see manual page */
	}
	while ((waitval = wait(&exitstatus)) != pid)
	{
		if (waitval == -1)
			break;
	}
	if (waitval == -1)
	{
		/* no child ??? or maybe interrupted ??? */
		exitstatus = -1;
	}
	if (!str)
	{
		if (exitstatus == FAIL << 8) /* execve() failed */
			exitstatus = 0;
		else
			exitstatus = 1; /* /bin/sh exists */
	}
	return exitstatus;
}

#endif
