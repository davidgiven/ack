#include <lib.h>
#define creat	_creat

PUBLIC int creat(name, mode)
char *name;
mode_t mode;
{
  return(_callm3(FS, CREAT, mode, name));
}
