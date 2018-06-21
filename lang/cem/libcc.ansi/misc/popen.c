/*
 * popen - open a pipe
 */
/* $Id$ */

#include <errno.h>
#include <stdio.h>
#include <signal.h>
#if defined(__BSD4_2)
union wait {
	int w_status;
};
typedef union wait wait_arg;
#else
typedef int wait_arg;
#endif /* __BSD4_2 */
#include "../stdio/loc_incl.h"

int _close(int d);
#if defined(__USG)
static
#endif
    int
    _dup2(int oldd, int newd); /* not present in System 5 */
int _execl(const char* name, ...);
int _fork(void);
int _pipe(int fildes[2]);
int _wait(wait_arg* status);
void _exit(int status);

static int pids[FOPEN_MAX];

FILE* popen(const char* command, const char* type)
{
	int piped[2];
	int Xtype = *type == 'r' ? 0 : *type == 'w' ? 1 : 2;
	int pid;

	if (Xtype == 2 || _pipe(piped) < 0 || (pid = _fork()) < 0)
		return 0;

	if (pid == 0)
	{
		/* child */
		register int* p;

		for (p = pids; p < &pids[FOPEN_MAX]; p++)
		{
			if (*p)
				_close(p - pids);
		}
		_close(piped[Xtype]);
		_dup2(piped[!Xtype], !Xtype);
		_close(piped[!Xtype]);
		_execl("/bin/sh", "sh", "-c", command, (char*)0);
		_exit(127); /* like system() ??? */
	}

	pids[piped[Xtype]] = pid;
	_close(piped[!Xtype]);
	return fdopen(piped[Xtype], type);
}

#if defined(__BSD4_2)
#define ret_val status.w_status
#else
#define ret_val status
#endif

int pclose(FILE* stream)
{
	int fd = fileno(stream);
	wait_arg status;
	int wret;
	void (*intsave)(int) = signal(SIGINT, SIG_IGN);
	void (*quitsave)(int) = signal(SIGQUIT, SIG_IGN);

	fclose(stream);
	while ((wret = _wait(&status)) != -1)
	{
		if (wret == pids[fd])
			break;
	}
	if (wret == -1)
		ret_val = -1;
	signal(SIGINT, intsave);
	signal(SIGQUIT, quitsave);
	pids[fd] = 0;
	return ret_val;
}

#if defined(__USG)
int _dup(int fildes);

static int
_dup2(int oldd, int newd)
{
	int i = 0, fd, tmp;
	int fdbuf[FOPEN_MAX];

	/* ignore the error on the close() */
	tmp = errno;
	(void)_close(newd);
	errno = tmp;
	while ((fd = _dup(oldd)) != newd)
	{
		if (fd == -1)
			break;
		fdbuf[i++] = fd;
	}
	tmp = errno;
	while (--i >= 0)
	{
		_close(fdbuf[i]);
	}
	errno = tmp;
	return -(fd == -1);
}
#endif /* __USG */
