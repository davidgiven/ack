/*
 * getpass - ask for a password
 */
/* $Id$ */

#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sgtty.h>
#include <fcntl.h>

int _open(const char* path, int flags);
int _write(int d, const char* buf, int nbytes);
int _read(int d, char* buf, int nbytes);
int _close(int d);

int _stty(int, struct sgttyb*);
int _gtty(int, struct sgttyb*);

char* getpass(const char* prompt)
{
	int i = 0;
	struct sgttyb tty, ttysave;
	static char pwdbuf[9];
	int fd;
	void (*savesig)(int);

	if ((fd = _open("/dev/tty", O_RDONLY)) < 0)
		fd = 0;
	savesig = signal(SIGINT, SIG_IGN);
	_write(2, prompt, strlen(prompt));
	_gtty(fd, &tty);
	ttysave = tty;
	tty.sg_flags &= ~ECHO;
	_stty(fd, &tty);
	i = _read(fd, pwdbuf, 9);
	while (pwdbuf[i - 1] != '\n')
		_read(fd, &pwdbuf[i - 1], 1);
	pwdbuf[i - 1] = '\0';
	_stty(fd, &ttysave);
	_write(2, "\n", 1);
	if (fd != 0)
		_close(fd);
	signal(SIGINT, savesig);
	return (pwdbuf);
}
