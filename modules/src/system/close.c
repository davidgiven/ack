/* $Header$ */

#include <system.h>

sys_close(fp)
	register File *fp;
{
	fp->o_flags = 0;
	close(fp->o_fd);
	fp->o_fd = -1;
}
