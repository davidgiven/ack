/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
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
	sys_write(fd, s, 1);
}

int
_BadAssertion(file, lineno, assertion)
	char *file, *assertion;
	int lineno;
{

	sys_write(STDERR, file, strlen(file));
	sys_write(STDERR, ", line ", 7);
	wr_num(STDERR, lineno);
	sys_write(STDERR, ": assertion \"", 13);
	sys_write(STDERR, assertion, strlen(assertion));
	sys_write(STDERR, "\" failed\n", 9);
	sys_stop(S_ABORT);
	return 0;
}
