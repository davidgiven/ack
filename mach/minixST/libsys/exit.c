#include <lib.h>

PUBLIC int exit(status)
int status;
{
  _cleanup();
  _exit(status);
}
