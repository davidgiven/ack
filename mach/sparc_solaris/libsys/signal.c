#include <errno.h>

extern int errno;

#define MAXSIG	128
void	(*_sig_funcs[MAXSIG])();
void	_sig_catch();
void	(*_signal())();

void
(*
signal(sig, handler))()
	void	(*handler)();
{
	void	(*old)(), (*retval)();

	if (sig <= 0 || sig > MAXSIG) {
		errno = EINVAL;
		return (void (*)()) -1;
	}
	if ((long) handler >= 0 && (long) handler <= 2) {
		retval = _signal(sig, handler);
		if (retval != (void (*)()) -1) {
			old = _sig_funcs[sig-1];
			_sig_funcs[sig-1] = handler;
			return old;
		}
		return retval;
	}
	retval = _signal(sig, _sig_catch);
	if (retval != (void (*)()) -1) {
		old = _sig_funcs[sig-1];
		_sig_funcs[sig-1] = handler;
		return old;
	}
	return retval;
}
