#include <lib.h>
#define dup	_dup
#include <unistd.h>
#include <fcntl.h>

PUBLIC int dup(fd)
int fd;
{
  return(fcntl(fd, F_DUPFD, 0));
}
