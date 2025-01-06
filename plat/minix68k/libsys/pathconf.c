/* POSIX pathconf (Sec. 5.7.1) 		Author: Andy Tanenbaum */

#include <lib.h>
#include <sys/types.h>
#define open	_open
#include <fcntl.h>
#include <errno.h>
#define close	_close

PUBLIC long pathconf(path, name)
char *path;			/* name of file being interrogated */
int name;			/* property being inspected */
{
/* POSIX allows some of the values in <limits.h> to be increased at
 * run time.  The pathconf and fpathconf functions allow these values
 * to be checked at run time.  MINIX does not use this facility.
 * The run-time limits are those given in <limits.h>.
 */

  int fd;
  long val;

  if ( (fd = open(path, O_RDONLY)) < 0) return(-1L);
  val = fpathconf(fd, name);
  close(fd);
  return(val);
}
