/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <cpm.h>

int read(int fd, void* buffer, size_t count)
{
	char i;
	
	/* We're only allowed to read from fd 0, 1 or 2. */
	
	if ((fd < 0) || (fd > 2))
	{
		errno = EBADF;
		return -1;
	}
	
	/* Empty buffer? */
	
	if (count == 0)
		return 0;
	
	/* Read one byte. */
	
	cpm_bc_register = CPM_BDOS_CONSOLE_INPUT;
	cpm_bdos();

	if (cpm_a_register == '\r') 
		cpm_a_register = '\n';
	*(char*)buffer = cpm_a_register;
	
	return 1;
}
