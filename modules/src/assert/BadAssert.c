/* _BadAssertion: used for debugging purposes. It should give an error message
	indicated by the parameters, and then give a core dump
*/

#include <system.h>

static
wr_num(fd, n)
	File *fd;
	int n;
{
	char s[2];

	s[1] = '\0';
	if (n >= 10) {
		wr_num(fd, n/10);
	}
	s[0] = (n % 10) + '0';
	(void) sys_write(fd, s, 1);
}

_BadAssertion(file, lineno, assertion)
	char *file, *assertion;
	int lineno;
{

	(void) sys_write(STDERR, file, strlen(file));
	(void) sys_write(STDERR, ", line ", 7);
	wr_num(STDERR, lineno);
	(void) sys_write(STDERR, ": assertion \"", 13);
	(void) sys_write(STDERR, assertion, strlen(assertion));
	(void) sys_write(STDERR, "\" failed\n", 9);
	sys_stop(S_ABORT);
}
