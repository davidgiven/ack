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
  int n, ispeed, ospeed;
  long erase, kill, intr, quit, xon, xoff, eof, brk, speed;

  M.TTY_REQUEST = request;
  M.TTY_LINE = fd;

  switch(request) {
     case TIOCSETP:
	erase = u.argp->sg_erase & BYTE;
	kill = u.argp->sg_kill & BYTE;
	M.TTY_SPEK = (erase << 8) | kill;
	M.TTY_FLAGS = u.argp->sg_flags;
	M.TTY_SPEED = (u.argp->sg_ospeed << 8) | u.argp->sg_ispeed;
	n = callx(FS, IOCTL);
  	return(n);
 
     case TIOCSETC:
  	intr = u.argt->t_intrc & BYTE;
  	quit = u.argt->t_quitc & BYTE;
  	xon  = u.argt->t_startc & BYTE;
  	xoff = u.argt->t_stopc & BYTE;
  	eof  = u.argt->t_eofc & BYTE;
  	brk  = u.argt->t_brkc & BYTE;		/* not used at the moment */
  	M.TTY_SPEK = (intr<<24) | (quit<<16) | (xon<<8) | (xoff<<0);
  	M.TTY_FLAGS = (eof<<8) | (brk<<0);
  	n = callx(FS, IOCTL);
  	return(n);
  	
     case TIOCGETP:
  	n = callx(FS, IOCTL);
	u.argp->sg_erase = (M.TTY_SPEK >> 8) & BYTE;
	u.argp->sg_kill  = (M.TTY_SPEK >> 0) & BYTE;
  	u.argp->sg_flags = M.TTY_FLAGS & 0xFFFF;
	speed = (M.TTY_FLAGS >> 16) & 0xFFFFL;
	u.argp->sg_ispeed = speed & BYTE;
	u.argp->sg_ospeed = (speed >> 8) & BYTE;
  	return(n);

     case TIOCGETC:
  	n = callx(FS, IOCTL);
  	u.argt->t_intrc  = (M.TTY_SPEK >> 24) & BYTE;
  	u.argt->t_quitc  = (M.TTY_SPEK >> 16) & BYTE;
  	u.argt->t_startc = (M.TTY_SPEK >>  8) & BYTE;
  	u.argt->t_stopc  = (M.TTY_SPEK >>  0) & BYTE;
  	u.argt->t_eofc   = (M.TTY_FLAGS >> 8) & BYTE;
  	u.argt->t_brkc   = (M.TTY_FLAGS >> 8) & BYTE;
  	return(n);

     default:
	n = -1;
	errno = -(EINVAL);
	return(n);
  }
}

