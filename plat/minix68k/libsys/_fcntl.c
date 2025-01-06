#include <lib.h>
#define fcntl _fcntl
#include <fcntl.h>
#if _ANSI
#endif

#if _ANSI
#include <stdarg.h>
PUBLIC int fcntl(int fd, int cmd, ...)
{
#else
#include <varargs.h>
PUBLIC int fcntl(va_alist)
va_dcl
{
int fd;
int cmd;
#endif
  va_list ap;
  int int3;			/* third integer parameter for callm1 */
  char *ptr1;			/* first pointer parameter for callm1 */

#if _ANSI
  va_start(ap, cmd);
#else
  va_start(ap);
  fd = va_arg(ap, int);
  cmd = va_arg(ap, int);
#endif

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
	int3 = va_arg(ap, int);
	break;
     case F_GETLK:
     case F_SETLK:
     case F_SETLKW:
	ptr1 = (char *) va_arg(ap, struct flock *);
	break;
  }

  /* Clean up and make the system call. */  
  va_end(ap);	
  return(_callm1(FS, FCNTL, fd, cmd, int3, ptr1, NIL_PTR, NIL_PTR));
}
