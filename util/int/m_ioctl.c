/*
	Dedicated to the ioctl system call, MON 54.
*/

/* $Id$ */

#include	"sysidf.h"
#include	"v7ioctl.h"
#include	"global.h"
#include	"mem.h"
#include	"warn.h"
#include	"whatever.h"

#ifdef WANT_SGTTY
#include	<sgtty.h>
#endif

#ifdef	V7IOCTL				/* define the proper V7 requests */

#define	V7IOGETP	(('t'<<8)|8)
#define	V7IOSETP	(('t'<<8)|9)
#define	V7IOSETN	(('t'<<8)|10)
#define	V7IOEXCL	(('t'<<8)|13)
#define	V7IONXCL	(('t'<<8)|14)
#define	V7IOHPCL	(('t'<<8)|2)
#define	V7IOFLUSH	(('t'<<8)|16)

#define	V7IOSETC	(('t'<<8)|17)
#define	V7IOGETC	(('t'<<8)|18)

#endif	/* V7IOCTL */


/************************************************************************
 * do_ioctl handles all ioctl system calls. It is called by the		*
 * moncall() routine, case 54.  It was too big to leave it there.	*
 * The ioctl system call is divided into 5 parts.			*
 * Ioctl's dealing with respectively:					*
 * sgttyb, tchars, local mode word, ltchars, and miscellaneous ioctl's.	*
 * Some of the sgttyb-calls are only possible under the new tty-driver.	*
 * All of these are to be found in the miscellaneous section.		*
 * do_ioctl() simply returns the value ioctl() would return itself.	*
 * (0 for success, -1 for failure)					*
 ***********************************************************************/

int do_ioctl(int fd, int req, ptr addr)
{
	register long e;
#ifdef WANT_SGTTY
	struct sgttyb sg_buf;
#endif

#ifdef	BSD_X				/* from system.h */
#ifndef	V7IOCTL
	char c;
	int mask;	/* will get ALIGNMENT problems with this one */
	long count;	/* might get ALIGNMENT problems with this one */
	int ldisc;	/* might get ALIGNMENT problems with this one */
	int pgrp;	/* might get ALIGNMENT problems with this one */
#endif	/* V7IOCTL */

	struct tchars tc_buf;
#ifndef	V7IOCTL
	struct ltchars ltc_buf;
#endif	/* V7IOCTL */
#endif	/* BSD_X */


#ifdef	V7IOCTL
	switch (req) {			/* translate the V7 requests */
					/* and reject the non-V7 ones */
					
#ifdef	WANT_SGTTY
	case V7IOGETP:
		req = TIOCGETP;
		break;
	case V7IOSETP:
		req = TIOCSETP;
		break;
	case V7IOEXCL:
		req = TIOCEXCL;
		break;
	case V7IONXCL:
		req = TIOCNXCL;
		break;
	case V7IOHPCL:
		req = TIOCHPCL;
		break;
#endif

#ifdef	BSD_X				/* from system.h */
	case V7IOSETN:
		req = TIOCSETN;
		break;

	case V7IOSETC:
		req = TIOCSETC;
		break;
	case V7IOGETC:
		req = TIOCGETC;
		break;
#endif	/* BSD_X */

	default:
		einval(WBADIOCTL);
		return (-1);			/* Fake return value */
	}

#endif	/* V7IOCTL */


	switch (req) {

		/*************************************/
		/****** Struct sgttyb ioctl's ********/
		/*************************************/

#ifdef WANT_SGTTY
	case TIOCGETP:
		/* Get fd's current param's and store at dsp2 */
		if (	(e = ioctl(fd, req, (char *) &sg_buf)) == -1
		||	!sgttyb2mem(addr, &sg_buf)
		) {
			e = -1;		/* errno already set */
		}
		break;

	case TIOCSETP:
#ifdef	BSD4_1				/* from system.h */
	case TIOCSETN:
#endif	/* BSD4_1 */
		/* set fd's parameters according to sgtty buffer	*/
		/* pointed to (addr), so first fill sg_buf properly.	*/
		if (	!mem2sgtty(addr, &sg_buf)
		||	(e = ioctl(fd, req, (char *) &sg_buf)) == -1
		) {
			e = -1;		/* errno already set */
		}
		break;

	case TIOCEXCL:
	case TIOCNXCL:
	case TIOCHPCL:
		/* These have no third argument. */
		e = ioctl(fd, req, (char *) 0);
		break;
#endif

#ifdef	BSD_X				/* from system.h */
		/*************************************/
		/****** Struct tchars ioctl's ********/
		/*************************************/

	case TIOCGETC:
		/* get special char's; store at addr */
		if (	(e = ioctl(fd, req, (char *) &tc_buf)) == -1
		||	!tchars2mem(addr, &tc_buf)
		) {
			e = -1;		/* errno already set */
		}
		break;

	case TIOCSETC:
		/* set special char's; load from addr */
		if (	!mem2tchars(addr, &tc_buf)
		||	(e = ioctl(fd, req, (char *) &tc_buf)) == -1
		) {
			e = -1;
		}
		break;

#ifndef	V7IOCTL
		/***************************************/
		/****** Local mode word ioctl's ********/
		/***************************************/

	case TIOCLBIS:	/* addr points to mask which is or-ed with lmw */
	case TIOCLBIC:	/* addr points to mask, ~mask & lmw is done */
	case TIOCLSET:	/* addr points to mask, lmw is replaced by it */
		if (memfault(addr, wsize)) {
			e = -1;
		}
		else {
			mask = mem_ldu(addr, wsize);
			e = ioctl(fd, req, (char *) &mask);
		}
		break;

	case TIOCLGET:	/* addr points to space, store lmw there */
		if (	memfault(addr, wsize)
		||	(e = ioctl(fd, req, (char *) &mask)) == -1
		) {
			e = -1;
		}
		else {
			mem_stn(addr, (long) mask, wsize);
		}
		break;

		/**************************************/
		/****** Struct ltchars ioctl's ********/
		/**************************************/

	case TIOCGLTC:
		/* get current ltc's; store at addr */
		if (	(e = ioctl(fd, req, (char *) &ltc_buf)) == -1
		||	!ltchars2mem(addr, &ltc_buf)
		) {
			e = -1;		/* errno already set */
		}
		break;

	case TIOCSLTC:
		/* set ltc_buf; load from addr */
		if (	!mem2ltchars(addr, &ltc_buf)
		||	(e = ioctl(fd, req, (char *) &ltc_buf)) == -1
		) {
			e = -1;
		}
		break;

		/*************************************/
		/****** Miscellaneous ioctl's ********/
		/*************************************/

	case TIOCGETD:
		/* Get line discipline, store at addr */
		if (	memfault(addr, wsize)
		||	(e = ioctl(fd, req, (char *) &ldisc)) == -1
		) {
			e = -1;
		}
		else {
			mem_stn(addr, (long) ldisc, wsize);
		}
		break;

	case TIOCSETD:
		/* Set line discipline, load from addr */
		if (memfault(addr, wsize)) {
			e = -1;
		}
		else {
			ldisc = (int) mem_ldu(addr, wsize);
			e = ioctl(fd, req, (char *) &ldisc);
		}
		break;

	/* The following are not standard vanilla 7 UNIX */
	case TIOCSBRK:	/* These have no argument */
	case TIOCCBRK:	/* They work on parts of struct sgttyb */
	case TIOCSDTR:
	case TIOCCDTR:
		e = ioctl(fd, req, (char *) 0);
		break;

	/* The following are used to set the line discipline */
	case OTTYDISC:
	case NETLDISC:
	case NTTYDISC:
		e = ioctl(fd, req, (char *) 0);
		break;

	case TIOCSTI:	/* addr = address of character */
		if (memfault(addr, 1L)) {
			e = -1;
		}
		else {
			c = (char) mem_ldu(addr, 1L);
			e = ioctl(fd, req, (char *) &c);
		}
		break;

	case TIOCGPGRP:
		/* store proc grp number of control term in addr */
		if (	memfault(addr, wsize)
		||	(e = ioctl(fd, req, (char *) &pgrp)) == -1
		) {
			e = -1;
		}
		else {
			mem_stn(addr, (long) pgrp, wsize);
		}
		break;

	case TIOCSPGRP:	/* addr is NO POINTER !! */
		e = ioctl(fd, req, (char *) addr);
		break;

	case FIONREAD:	/* do the ioctl, addr is long-int ptr now */
		if (	memfault(addr, wsize)
		||	(e = ioctl(fd, req, (char *) &count)) == -1
		) {
			e = -1;
		}
		else {
			mem_stn(addr, count, wsize);
		}
		break;

#endif	/* V7IOCTL */
#endif	/* BSD_X */

	default:
		einval(WBADIOCTL);
		e = -1;			/* Fake return value */
		break;
	}
	return (e);
}
