/* $Id$ */
#define UTMPFILE "/etc/utmp"

#ifdef __USG
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
#else
struct utmp {
	char	ut_line[8];
	char	ut_name[8];
#ifdef __BSD4_2
	char	ut_host[16];
#endif
	long	ut_time;
};
#endif

char *
getlogin()
{
	struct utmp ut;
	static char name[sizeof(ut.ut_name) + 1];
	int	slotno = ttyslot();
	int	fd;
	register char *p, *q;

	if (! slotno || !(fd = open(UTMPFILE, 0))) return 0;
	lseek(fd, (long) slotno * sizeof(ut), 0);
	if (read(fd, (char *) &ut, sizeof(ut)) < sizeof(ut)) return 0;
	close(fd);
	ut.ut_name[sizeof(ut.ut_name)] = ' ';
	p = ut.ut_name;
	q = name;
	while (*p != ' ') *q++ = *p++;
	*q = '\0';
	return name;
}
