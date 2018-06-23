/*
 * popen - open a pipe
 */
/* $Id$ */

#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#if ACKCONF_WANT_EMULATED_POPEN

static int pids[FOPEN_MAX];

FILE* popen(const char* command, const char* type)
{
	int piped[2];
	int Xtype = *type == 'r' ? 0 : *type == 'w' ? 1 : 2;
	int pid;

	if (Xtype == 2 || pipe(piped) < 0 || (pid = fork()) < 0)
		return 0;

	if (pid == 0)
	{
		/* child */
		register int* p;

		for (p = pids; p < &pids[FOPEN_MAX]; p++)
		{
			if (*p)
				close(p - pids);
		}
		close(piped[Xtype]);
		dup2(piped[!Xtype], !Xtype);
		close(piped[!Xtype]);
		execl("/bin/sh", "sh", "-c", command, (char*)0);
		_exit(127); /* like system() ??? */
	}

	pids[piped[Xtype]] = pid;
	close(piped[!Xtype]);
	return fdopen(piped[Xtype], type);
}

int pclose(FILE* stream)
{
	int fd = fileno(stream);
	int status;
	int wret;
	void (*intsave)(int) = signal(SIGINT, SIG_IGN);
	void (*quitsave)(int) = signal(SIGQUIT, SIG_IGN);

	fclose(stream);
	while ((wret = wait(&status)) != -1)
	{
		if (wret == pids[fd])
			break;
	}
	if (wret == -1)
		status = -1;
	signal(SIGINT, intsave);
	signal(SIGQUIT, quitsave);
	pids[fd] = 0;
	return status;
}

#endif
