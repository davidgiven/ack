/* _utime(2) for POSIX		Authors: Terrence W. Holm & Edwin L. Froese */

#include <lib.h>
#define time	_time
#include <time.h>
#define utime	_utime
#include <utime.h>

long time();

PUBLIC int utime(name, timp)
char *name;
struct utimbuf *timp;
{
  long current_time;

  if (timp == (struct utimbuf *)NULL) {
	current_time = time((long *)NULL);
	_M.m2_l1 = current_time;
	_M.m2_l2 = current_time;
  } else {
	_M.m2_l1 = timp->actime;
	_M.m2_l2 = timp->modtime;
  }

  _M.m2_i1 = _len(name);
  _M.m2_p1 = name;
  return _callx(FS, UTIME);
}
