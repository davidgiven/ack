/* $Id$ */
#include <signal.h>
#include <sgtty.h>

char * getpass(prompt)
char *prompt;
{
	int i = 0;
	struct sgttyb tty, ttysave;
	static char pwdbuf[9];
	int fd;
	void (*savesig)();

	if ((fd = open("/dev/tty", 0)) < 0) fd = 0;
	savesig = signal(SIGINT, SIG_IGN);
	write(2, prompt, strlen(prompt));
	gtty(fd, &tty);
	ttysave = tty;
	tty.sg_flags &= ~ECHO;
	stty(fd, &tty);
	i = read(fd, pwdbuf, 9);
	while (pwdbuf[i - 1] != '\n')
		read(fd, &pwdbuf[i - 1], 1);
	pwdbuf[i - 1] = '\0';
	stty(fd, &ttysave);
	write(2, "\n", 1);
	if (fd != 0) close(fd);
	signal(SIGINT, savesig);
	return(pwdbuf);
}
