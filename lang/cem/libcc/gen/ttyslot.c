#ifdef USG
/* some systems require inclusion of sys/types.h before utmp.h */
#include <sys/types.h>
#include <utmp.h>

/*	system V, so no /etc/ttys file. In this case, scan the
	/etc/utmp file
*/
#define FILENAME "/etc/utmp"
#else
#define FILENAME "/etc/ttys"
#endif

char    *ttyname();
char    *rindex();

ttyslot()
{
	register char *tp, *p;
	int fd;
	int retval = 1;
#ifdef USG
	struct utmp buf;
#else
	char buf[32];
#endif

	if (! (tp=ttyname(0)) && ! (tp=ttyname(1)) && !(tp=ttyname(2)))
		return 0;
	if (! (p = rindex(tp, '/')))
		p = tp;
	else
		p++;
	if ((fd = open(FILENAME, 0)) < 0) return 0;
#ifdef USG
	while (read(fd, (char *) &buf, sizeof(buf)) == sizeof(buf)) {
		if ((buf.ut_type == INIT_PROCESS ||
		     buf.ut_type == LOGIN_PROCESS ||
		     buf.ut_type == USER_PROCESS ||
		     buf.ut_type == DEAD_PROCESS) &&
		    ! strncmp(buf.ut_line, p, sizeof(buf.ut_line))) {
			close(fd);
			return retval;
		}
		retval++;
	}
	close(fd);
	return 0;
#else
	for (;;) {
		tp = buf;
		for (;;) {
			if (read(fd, tp, 1) != 1) {
				close(fd);
				return 0;
			}
			if (*tp == '\n' || tp >= &buf[31]) {
				*tp = 0;
				tp = buf+2;
				break;
			}
		}
		if (! strcmp(p, tp)) {
			close(fd);
			return retval;
		}
		retval++;
	}
	/*NOTREACHED*/
#endif
}
