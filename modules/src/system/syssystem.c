#include <stdlib.h>
#include <string.h>
#include "system.h"

#if defined WIN32
	#define ESCAPECHAR '^'

	static int needs_escaping(char c)
	{
		switch (c)
		{
			case ' ':
			case '"':
			case '\'':
			case '(':
			case ')':
			case '^':
				return 1;

			default:
				return 0;
		}
	}
#else
	#define ESCAPECHAR '\\'

	static int needs_escaping(char c)
	{
		switch (c)
		{
			case ' ':
			case '"':
			case '\'':
			case '\\':
			case '(':
			case ')':
				return 1;

			default:
				return 0;
		}
	}
#endif

static char* append_escaped(char* buffer, const char* word)
{
	for (;;)
	{
		char c = *word++;
		if (!c)
			break;
		if (needs_escaping(c))
			*buffer++ = ESCAPECHAR;
		*buffer++ = c;
	}
	return buffer;
}

int sys_system(const char* prog, const char* const* arglist)
{
	/* Calculate the maximum length of the command line. */

	int len = strlen(prog) * 2 + 1;
	for (const char* const* arg = arglist+1; *arg; arg++)
		len += strlen(*arg) * 2 + 1;

	/* Now actually build the command line. */

	char* cmdline = malloc(len + 1);
	char* p = append_escaped(cmdline, prog);

	for (const char* const* arg = arglist+1; *arg; arg++)
	{
		const char* word = *arg;
		*p++ = ' ';

		p = append_escaped(p, word);
	}

	*p = 0;

	int status = system(cmdline);

	free(cmdline);
	return status;
}

