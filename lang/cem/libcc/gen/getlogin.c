#define UTMPFILE "/etc/utmp"

/* some systems require inclusion of sys/types.h before utmp.h */
#include <sys/types.h>
#include <utmp.h>

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
