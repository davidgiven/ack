/* $Id$ */
#ifdef __USG
/*	system V, so no /etc/ttys file. In this case, scan the
	/etc/utmp file
*/
struct utmp {
	char ut_name[8];
	char ut_id[4];
	char ut_line[12];
	short ut_pid;
	short ut_type;
	struct exit_status {
		short e_termination;
		short e_exit;
	} ut_exit;
	long ut_time;
};
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
#ifdef __USG
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
#ifdef __USG
	while (read(fd, (char *) &buf, sizeof(buf)) == sizeof(buf)) {
		/* processes associated with a terminal ...
		   unfortunately we cannot use the include file because
		   some systems have a different one ...
		   INIT_PROCESS, DEAD_PROCESS, USER_PROCESS, LOGIN_PROCESS
		*/
		if ((buf.ut_type >= 5 && buf.ut_type <= 8) &&
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
		for (;;tp++) {
			if (read(fd, tp, 1) != 1) {
				close(fd);
				return 0;
			}
			if (*tp == '\n' || tp >= &buf[31]) {
				*tp = 0;
				if (tp < buf+2) buf[2] = '\0';
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
