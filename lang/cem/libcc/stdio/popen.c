#include <stdio.h>
#include <signal.h>

static int pids[20];

FILE *
popen(command, type)
	char *command, *type;
{
	int piped[2];
	int Xtype = *type == 'r' ? 0 : *type == 'w' ? 1 : 2;
	int pid;

	if (Xtype == 2 ||
	    pipe(piped) < 0 ||
	    (pid = fork()) < 0) return 0;
	
	if (pid == 0) {
		/* child */
		register int *p;

		for (p = pids; p < &pids[20]; p++) {
			if (*p) close(p - pids);
		}
		close(piped[Xtype]);
		dup2(piped[!Xtype], !Xtype);
		close(piped[!Xtype]);
		execl("/bin/sh", "sh", "-c", command, (char *) 0);
		_exit(127);	/* like system() ??? */
	}

	pids[piped[Xtype]] = pid;
	close(piped[!Xtype]);
	return fdopen(piped[Xtype], type);
}

pclose(iop)
	FILE *iop;
{
	int fd = fileno(iop);
	int status, wret;
	int (*intsave)() = signal(SIGINT, SIG_IGN);
	int (*quitsave)() = signal(SIGQUIT, SIG_IGN);

	fclose(iop);
	while ((wret = wait(&status)) != -1) {
		if (wret == pids[fd]) break;
	}
	if (wret == -1) status = -1;
	signal(SIGINT, intsave);
	signal(SIGQUIT, quitsave);
	pids[fd] = 0;
	return status;
}
