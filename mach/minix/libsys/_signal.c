#include <lib.h>
#define signal	_signal
#include <signal.h>

extern _PROTOTYPE(void (*_vectab[_NSIG]), (int));	/* array of funcs to catch signals */

/* The definition of signal really should be
 *  PUBLIC void (*signal(signr, func))()
 * but some compilers refuse to accept this, even though it is correct.
 * The only thing to do if you are stuck with such a defective compiler is
 * change it to
 *  PUBLIC void *signal(signr, func)
 * and change ../h/signal.h accordingly.
 */

PUBLIC void (*signal(signr, func))()
int signr;			/* which signal is being set */
_PROTOTYPE( void (*func), (int));	/* pointer to function that catches signal */
{
  int r;
  _PROTOTYPE( void (*old), (int));

  old = _vectab[signr - 1];
  _M.m6_i1 = signr;
  if (func == SIG_IGN || func == SIG_DFL)
	/* Keep old signal catcher until it is completely de-installed */
	_M.m6_f1 = func;
  else {
	/* Use new signal catcher immediately (old one may not exist) */
	_vectab[signr - 1] = func;
	_M.m6_f1 = _begsig;
  }
  r = _callx(MM, SIGNAL);
  if (r < 0) {
	_vectab[signr - 1] = old;/* undo any pre-installation */
	return((void (*) ()) r);
  }
  _vectab[signr - 1] = func;	/* redo any pre-installation */
  if (r == 1) return(SIG_IGN);
  return(old);
}
