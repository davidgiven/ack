#include <lib.h>
#include <unistd.h>

PUBLIC void _exit(status)
int status;
{
  _callm1(MM, EXIT, status, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
}
