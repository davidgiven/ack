#include "lib.h"
#include <minix/com.h>
#include <sgtty.h>

PUBLIC int ioctl(fd, request, u)
int fd;
int request;
union {
  struct sgttyb *argp;
  struct tchars *argt;
} u;

{
  int n;
  long erase, kill, intr, quit, xon, xoff, eof, brk;

  M.TTY_REQUEST = request;
  M.TTY_LINE = fd;

  switch(request) {
     case TIOCSETP:
	erase = u.argp->sg_erase & 0377;
	kill = u.argp->sg_kill & 0377;
	M.TTY_SPEK = (erase << 8) | kill;
	M.TTY_FLAGS = u.argp->sg_flags;
	n = callx(FS, IOCTL);
  	return(n);
 
     case TIOCSETC:
  	intr = u.argt->t_intrc & 0377;
  	quit = u.argt->t_quitc & 0377;
  	xon  = u.argt->t_startc & 0377;
  	xoff = u.argt->t_stopc & 0377;
  	eof  = u.argt->t_eofc & 0377;
  	brk  = u.argt->t_brkc & 0377;		/* not used at the moment */
  	M.TTY_SPEK = (intr<<24) | (quit<<16) | (xon<<8) | (xoff<<0);
  	M.TTY_FLAGS = (eof<<8) | (brk<<0);
  	n = callx(FS, IOCTL);
  	return(n);
  	
     case TIOCGETP:
  	n = callx(FS, IOCTL);
	u.argp->sg_erase = (M.TTY_SPEK >> 8) & 0377;
	u.argp->sg_kill  = (M.TTY_SPEK >> 0) & 0377;
  	u.argp->sg_flags = M.TTY_FLAGS;
  	return(n);

     case TIOCGETC:
  	n = callx(FS, IOCTL);
  	u.argt->t_intrc  = (M.TTY_SPEK >> 24) & 0377;
  	u.argt->t_quitc  = (M.TTY_SPEK >> 16) & 0377;
  	u.argt->t_startc = (M.TTY_SPEK >>  8) & 0377;
  	u.argt->t_stopc  = (M.TTY_SPEK >>  0) & 0377;
  	u.argt->t_eofc   = (M.TTY_FLAGS >> 8) & 0377;
  	u.argt->t_brkc   = (M.TTY_FLAGS >> 8) & 0377;
  	return(n);

     default:
	n = -1;
	errno = -(EINVAL);
	return(n);
  }
}

