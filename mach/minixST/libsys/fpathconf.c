/* POSIX fpathconf (Sec. 5.7.1) 		Author: Andy Tanenbaum */

#include <lib.h>
#include <unistd.h>
#include <sys/types.h>
#define fstat	_fstat
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>

PUBLIC long fpathconf(fd, name)
int fd;				/* file descriptor being interrogated */
int name;			/* property being inspected */
{
/* POSIX allows some of the values in <limits.h> to be increased at
 * run time.  The pathconf and fpathconf functions allow these values
 * to be checked at run time.  MINIX does not use this facility.
 * The run-time limits are those given in <limits.h>.
 */

  struct stat stbuf;

  switch(name) {
	case _PC_LINK_MAX:
		/* Fstat the file.  If that fails, return -1. */
		if (fstat(fd, &stbuf) != 0) return(-1L);
		if (S_ISDIR(stbuf.st_mode))
			return(1L);	/* no links to directories */
		else
			return( (long) LINK_MAX);

	case _PC_MAX_CANON:
		return( (long) MAX_CANON);

	case _PC_MAX_INPUT:
		return( (long) MAX_INPUT);

	case _PC_NAME_MAX:
		return( (long) NAME_MAX);

	case _PC_PATH_MAX:
		return( (long) PATH_MAX);

	case _PC_PIPE_BUF:
		return( (long) PIPE_BUF);

	case _PC_CHOWN_RESTRICTED:
		return( (long) 1);	/* MINIX defines CHOWN_RESTRICTED */

	case _PC_NO_TRUNC:		/* MINIX does not define NO_TRUNC */
		return( (long) 0);

	case _PC_VDISABLE:		/* MINIX defines VDISABLE */
		return( (long) _POSIX_VDISABLE);

	default:
		errno = EINVAL;
		return(-1L);
  }
}
