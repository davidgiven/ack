#include <lib.h>
#include <sys/types.h>
#define open	_open
#include <fcntl.h>

#if _ANSI
#include <stdarg.h>

PUBLIC int open(const char *name, int flags, ...)
{
	int i;
	va_list ap;

	if (flags & O_CREAT) {
		va_start(ap, flags);
		i = va_arg(ap, int);
		i = _callm1(FS, OPEN, _len(name), flags, i,
			    (char *)name, NIL_PTR, NIL_PTR);
		va_end(ap);
		return i;
	}
	return _callm3(FS, OPEN, flags, name);
}
#else
PUBLIC int open(name, flags, mode)
char *name;
int flags, mode;
{
  if (flags & O_CREAT)
	return _callm1(FS, OPEN, _len(name), flags, mode,
		      (char *)name, NIL_PTR, NIL_PTR);
  return(_callm3(FS, OPEN, flags, name));
}
#endif
