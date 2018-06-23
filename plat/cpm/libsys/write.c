/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <cpm.h>

void _sys_write_tty(char c)
{
	cpm_bc_register = CPM_BDOS_CONSOLE_OUTPUT;
	cpm_de_register = c;
	cpm_bdos();

	if (c == '\n')
	{
		cpm_bc_register = CPM_BDOS_CONSOLE_OUTPUT;
		cpm_de_register = '\r';
		cpm_bdos();
	}
}

ssize_t write(int fd, void* buffer, size_t count)
{
	int i;
	char* p = buffer;
	
	/* We're only allowed to write to fd 0, 1 or 2. */
	
	if ((fd < 0) || (fd > 2))
	{
		errno = EBADF;
		return -1;
	}
	
	/* Write all data. */
	
	i = 0;
	while (i < count)
	{
		_sys_write_tty(*p++);
			
		i++;
	}
	
	/* No failures. */
	
	return count;
}
