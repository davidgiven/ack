#include <lib.h>
#include <minix/com.h>
#define ioctl	_ioctl
#include <sgtty.h>

PUBLIC int ioctl(fd, request, argp)
int fd;
int request;
struct sgttyb *argp;
{
  int n;
  long erase, kill, intr, quit, xon, xoff, eof, brk, speed;
  struct tchars *argt;

  _M.TTY_REQUEST = request;
  _M.TTY_LINE = fd;

  switch(request) {
     case TIOCSETP:
	erase = argp->sg_erase & BYTE;
	kill = argp->sg_kill & BYTE;
	speed = ((argp->sg_ospeed & BYTE) << 8) | (argp->sg_ispeed & BYTE);
	_M.TTY_SPEK = (speed << 16) | (erase << 8) | kill;
	_M.TTY_FLAGS = argp->sg_flags;
	n = _callx(FS, IOCTL);
  	return(n);
 
     case TIOCSETC:
	argt = (struct tchars * /* kludge */) argp;
  	intr = argt->t_intrc & BYTE;
  	quit = argt->t_quitc & BYTE;
  	xon  = argt->t_startc & BYTE;
  	xoff = argt->t_stopc & BYTE;
  	eof  = argt->t_eofc & BYTE;
  	brk  = argt->t_brkc & BYTE;		/* not used at the moment */
  	_M.TTY_SPEK = (intr<<24) | (quit<<16) | (xon<<8) | (xoff<<0);
  	_M.TTY_FLAGS = (eof<<8) | (brk<<0);
  	n = _callx(FS, IOCTL);
  	return(n);
  	
     case TIOCGETP:
  	n = _callx(FS, IOCTL);
	argp->sg_erase = (_M.TTY_SPEK >> 8) & BYTE;
	argp->sg_kill  = (_M.TTY_SPEK >> 0) & BYTE;
  	argp->sg_flags = _M.TTY_FLAGS & 0xFFFFL;
	speed = (_M.TTY_SPEK >> 16) & 0xFFFFL;
	argp->sg_ispeed = speed & BYTE;
	argp->sg_ospeed = (speed >> 8) & BYTE;
  	return(n);

     case TIOCGETC:
  	n = _callx(FS, IOCTL);
	argt = (struct tchars *) argp;
  	argt->t_intrc  = (_M.TTY_SPEK >> 24) & BYTE;
  	argt->t_quitc  = (_M.TTY_SPEK >> 16) & BYTE;
  	argt->t_startc = (_M.TTY_SPEK >>  8) & BYTE;
  	argt->t_stopc  = (_M.TTY_SPEK >>  0) & BYTE;
  	argt->t_eofc   = (_M.TTY_FLAGS >> 8) & BYTE;
  	argt->t_brkc   = (_M.TTY_FLAGS >> 8) & BYTE;
  	return(n);

/* This is silly, do we want to add 1001 cases and _M.TTY_XYZ's here?
 * We should just pop argp into the message for low-level interpretation.
 */

     case TIOCFLUSH:
	_M.TTY_FLAGS = (int /* kludge */) argp;
	return _callx(FS, IOCTL);

/* decided to pop argp in the ADDRESS field. Left TIOCFLUSH a special case
 * since it affects other platforms and old software too. FM
 */
     default:
	_M.ADDRESS = (char *)argp;
	return _callx(FS, IOCTL);
  }
}
