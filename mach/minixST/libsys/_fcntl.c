#include <lib.h>
#define fcntl _fcntl
#include <fcntl.h>
#include <stdarg.h>

#if _ANSI
PUBLIC int fcntl(int fd, int cmd, ...)
#else
PUBLIC int fcntl(fd, cmd)
int fd;
int cmd;
#endif
{
  va_list argp;
  int int3;			/* third integer parameter for callm1 */
  char *ptr1;			/* first pointer parameter for callm1 */

  va_start(argp, cmd);

  /* Set up for the sensible case where there is no variable parameter.  This
   * covers F_GETFD, F_GETFL and invalid commands.
   */
  int3 = 0;
  ptr1 = NIL_PTR;

  /* Adjust for the stupid cases. */
  switch(cmd) {
     case F_DUPFD:
     case F_SETFD:
     case F_SETFL:
	int3 = va_arg(argp, int);
	break;
     case F_GETLK:
     case F_SETLK:
     case F_SETLKW:
	ptr1 = (char *) va_arg(argp, struct flock *);
	break;
  }

  /* Clean up and make the system call. */  
  va_end(argp);	
  return(_callm1(FS, FCNTL, fd, cmd, int3, ptr1, NIL_PTR, NIL_PTR));
}
