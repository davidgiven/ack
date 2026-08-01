/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int isatty(int fd)
{
  return (fd>=0 && fd<=2);
}
