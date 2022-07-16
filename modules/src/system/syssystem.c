#include <stdlib.h>
#include <string.h>
#include "system.h"

int sys_system(const char* prog, const char* const* arglist)
{
	/* Calculate the maximum length of the command line. */

	int len = strlen(prog);
	for (const char* const* arg = arglist+1; *arg; arg++)
		len += strlen(*arg) * 2 + 1;

	/* Now actually build the command line. */

	char* cmdline = malloc(len + 1);
	strcpy(cmdline, prog);
	char* p = cmdline + strlen(prog);

	for (const char* const* arg = arglist+1; *arg; arg++)
	{
		const char* word = *arg;
		*p++ = ' ';

		for (;;)
		{
			char c = *word++;
			if (!c)
				break;
			if ((c == ' ') || (c == '\"') || (c == '\''))
				*p++ = '\\';
			*p++ = c;
		}
	}

	*p = 0;

	int status = system(cmdline);

	free(cmdline);
	return status;
}

