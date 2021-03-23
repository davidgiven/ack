/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define	OUT_OF_MEMORY (void*)(-1)	/* sbrk returns this on failure */

struct for_main {
	int argc;
	char **argv;
	char **envp;
};

/*
 * Marshal the command line and environment variable data provided by
 * MS-DOS, into the standard format expected by a C main(...) routine.  The
 * environment variable data has been copied to the near data segment.
 *
 * Return zero if everything went well, non-zero otherwise.
 */
int _sys_initmain(char *arg_data, size_t n_env_vars, char *env_data,
    unsigned msdos_ver, struct for_main *out)
{
	char **argv, **envp;
	char c, *p, **pp;
	unsigned char n;

	/*
	 * Allocate space for argv[] for the maximum possible number of
	 * arguments.  We can shrink this later.
	 */
	if ((argv = sbrk(65 * sizeof(char *))) == OUT_OF_MEMORY)
		return -1;
	out->argv = argv;

	/* Sanity-check the original command line. */
	p = arg_data;
	n = (unsigned char)*p;
	if (n > 0x7E || p[1 + n] != '\r')
		return -1;

	/* Initialize argv[0] to an empty string first. */
	*p = 0;
	argv[0] = p;

	/*
	 * Split the command line string into separate arguments.
	 * TODO: handle double-quoting, backslashes, etc.
	 */
	pp = argv + 1;
	do
	{
		do
			c = *++p;
		while (c == ' ' || c == '\t');

		if (c != '\r')
		{
			*pp++ = p;
			do
				c = *++p;
			while (c != ' ' && c != '\t' && c != '\r');
			*p = 0;
		}
	} while (c != '\r');

	/* Wrap up argv[].  Compute argc.  Free up unneeded space. */
	out->argc = pp - argv;
	*pp++ = NULL;
	if (brk(pp) != 0)
		return -1;

	/* Allocate space for envp[]. */
	if ((envp = sbrk((n_env_vars + 1) * sizeof(char *))) == OUT_OF_MEMORY)
		return -1;
	out->envp = envp;

	/*
	 * Populate envp[].  The environment data is simply a series of ASCIIZ
	 * strings, one after another, terminated by an empty string.
	 */
	pp = envp;
	p = env_data;
	while (*p)
	{
		*pp++ = p;
		while (*++p);
		++p;
	}
	*pp = NULL;

	/*
	 * If there is a program name (MS-DOS version is 3 or above), point
	 * argv[0] to it.
	 *
	 * p points to the zero byte just before the count of strings (a
	 * shortword) following the environment variables, so advance p by 3
	 * bytes to get at the program name.
	 */
	if ((msdos_ver & 0x00ff) >= 3)
	{
		p += 3;
		argv[0] = p;
	}

	/* We are done. */
	return 0;
}
