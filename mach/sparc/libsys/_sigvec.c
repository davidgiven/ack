#include "syscall.h"
#include <errno.h>
struct sigvec { int (*handler)(); int mask,flags; };
int (*(_sigfunc[32]))();
extern int _sigtramp();
extern int errno;

sigvec(sig,vec,ovec)
	register struct sigvec *vec;
	struct sigvec *ovec;
{
	struct sigvec tmp;
	int (*old)();

	if ((unsigned) sig >= 32) {
		errno = EINVAL;
		return -1;
	}
	old = _sigfunc[sig];
	if (vec) {
		tmp = *vec;
		vec = &tmp;
		if (vec->handler && vec->handler != (int (*)()) 1) {
			_sigfunc[sig] = vec->handler;
			vec->handler = _sigtramp;
		}
	}
	if (_sigvec_scall(sig,vec,ovec) < 0) {
		_sigfunc[sig] = old;
		return -1;
	}
	if (ovec && ovec->handler == _sigtramp) {
		ovec->handler = old;
	}
	return 0;
}
