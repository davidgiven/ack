/* $Id$ */
/*	channel.c - basic channel handling routines */
#include <errno.h>
#ifndef __BSD4_2
#include <signal.h>
#endif
#include <sgtty.h>
#include "ocm_chan.h"

static void disaster();

void c_init(c, z) register chan *c; register unsigned z;
/* Initialise an array of interprocess channels declared as: CHAN c[z]. */
{
	do {
		c->type=C_T_CHAN;
		(c++)->c.synch=C_S_FREE;
	} while (--z!=0);
}

void chan_in(v, c) long *v; register chan *c;
/* Reads a value from channel c and returns it through v. */
{
	switch(c->type) {
	case C_T_FILE:
		if ((c->f.flgs&C_F_READAHEAD)!=0) {
			*v=(c->f.preread&0377);
			c->f.flgs&= ~C_F_READAHEAD;
		} else {
			register FILE *fp= unix_file[c->f.index];

			*v= feof(fp) ? C_F_EOF : getc(fp);
		}
		break;
	case C_T_CHAN:
		deadlock=0;		/* Wait for value to arrive */
		while (c->c.synch!=C_S_ANY) resumenext();

		*v=c->c.val;
		c->c.synch=C_S_ACK;	/* Acknowledge receipt */
		break;
	default:
		disaster();
	}
}

void chan_out(v, c) long v; register chan *c;
/* Send value v through channel c. */
{
	switch(c->type) {
	case C_T_FILE: {
		register FILE *fp= unix_file[c->f.index];
		struct sgttyb tty;

		if ((v& ~0xff)==0)	/* Plain character */
			putc( (int) v, fp);
		else
		if (v==C_F_TEXT) {
			gtty(fileno(fp), &tty);
			tty.sg_flags&= ~CBREAK;
			tty.sg_flags|= ECHO|CRMOD;
			stty(fileno(fp), &tty);
		} else
		if (v==C_F_RAW) {
			gtty(fileno(fp),&tty);
			tty.sg_flags|= CBREAK;
			tty.sg_flags&= ~(ECHO|CRMOD);
			stty(fileno(fp), &tty);
		}
	}	break;
	case C_T_CHAN:
		deadlock=0;		/* Wait until channel is free */
		while (c->c.synch!=C_S_FREE) resumenext();

		c->c.val=v;
		c->c.synch=C_S_ANY;	/* Channel has data */

		deadlock=0;		/* Wait for acknowledgement */
		while (c->c.synch!=C_S_ACK) resumenext();

		c->c.synch=C_S_FREE;	/* Back to normal */
		break;
	default:
		disaster();
	}
}

#ifndef __BSD4_2
static void timeout();
#endif

int chan_any(c) register chan *c;
{
#ifdef __BSD4_2
#include <fcntl.h>
	int flags;
#endif
	switch (c->type) {
	case C_T_FILE:
		if ((c->f.flgs&C_F_READAHEAD)!=0)
			return 1;
		else {
			register FILE *fp= unix_file[c->f.index];
			
			if (feof(fp))
				return 1;
			else {
				extern int errno;
				register ch;

				deadlock=0;
					/* No deadlock while waiting for key */

				/* Unfortunately, the mechanism that was used
				   here does not work on all Unix systems.
				   On BSD 4.2 and newer, the "read" is 
				   automatically restarted. Therefore, on
				   these systems, we try it with non-blocking
				   reads
				*/
#ifdef __BSD4_2
				flags = fcntl(fileno(fp), F_GETFL, 0);
				fcntl(fileno(fp), F_SETFL, flags | O_NDELAY);
				errno = 0;
				ch = getc(fp);
				fcntl(fileno(fp), F_SETFL, flags);
				if (errno == EWOULDBLOCK) {
					clearerr(fp);
					return 0;
				}
#else
				signal(SIGALRM, timeout);
				alarm(1);

				errno=0;
				ch=getc(fp);

				signal(SIGALRM, SIG_IGN);
				alarm(0);

				if (errno==EINTR) {
					clearerr(fp);
					return 0;
				}
#endif
				else {
					if (!feof(fp)) {
						c->f.flgs|=C_F_READAHEAD;
						c->f.preread=ch;
					}
					return 1;
				}
			}
		}
	case C_T_CHAN:
		return c->c.synch==C_S_ANY;
	default:
		disaster();
	}
}

#ifndef __BSD4_2
/* The ch=getc(fp) in the above function calls read(2) to do its task, but if
 * there's no input on the file (pipe or terminal) then the read will block.
 * To stop this read from blocking, we use the fact that if the read is
 * interrupted by a signal that is caught by the program, then the read returns
 * error EINTR after the signal is processed. Thus we use a one second alarm
 * to interrupt the read with a trap to timeout(). But since the alarm signal
 * may occur *before* the read is called, it is continuously restarted in
 * timeout() to prevent it from getting lost.
 */

static void timeout(sig)
{
	signal(SIGALRM, timeout);
	alarm(1);
}
#endif

static void disaster()
{
	write(2, "Fatal error: Channel variable corrupted\n", 40);
	abort();
}
