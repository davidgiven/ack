/** @file
 *  Dedicated treatment of the sigtrp system call, MON 48.
 */

/* $Id$ */

#include	<signal.h>

#include	"global.h"
#include	"log.h"
#include	"warn.h"
#include	"trap.h"
#include	"m_sigtrp.h"
#include	"io.h"
#include	"whatever.h"

/*************************** SIGTRP *************************************
 *  The monitor call "sigtrp()" is handled by "do_sigtrp()".  The first	*
 *  argument is a EM-trap number (0<=tn<=252), the second a UNIX signal	*
 *  number.  The user wants trap "tn" to be generated, in case signal	*
 *  "sn" occurs.  The report about this interpreter has a section,	*
 *  giving all details about signal handling.  Do_sigtrp() returns the	*
 *  previous trap-number "sn" was mapped onto.  A return value of -1	*
 *  indicates an error.							*
 ************************************************************************/

#define	UNIX_trap(sn)	(SIGILL <= sn && sn <= SIGSYS)

#ifndef NSIG
#define NSIG _NSIG
#endif
PRIVATE int sig_map[NSIG + 1]; /* maps signals onto trap numbers */

PRIVATE void HndlIntSig(int); /* handle signal to interpreter */
PRIVATE void HndlEmSig(int); /* handle signal to user program */

void init_signals(void)
{
	int sn;

	for (sn = 0; sn < NSIG + 1; sn++)
	{
		sig_map[sn] = -2; /* Default EM trap number */
	}

	for (sn = 0; sn < NSIG + 1; sn++)
	{
		/* for all signals that would cause termination */
		if (!UNIX_trap(sn))
		{
#ifdef SIGCHLD
			if (sn == SIGCHLD)
				continue;
#endif
#ifdef SIGIO
			if (sn == SIGIO)
				continue;
#endif
#ifdef SIGWINCH
			if (sn == SIGWINCH)
				continue;
#endif
			if (signal(sn, SIG_IGN) != SIG_IGN)
			{
				/* we take our fate in our own hand */
				signal(sn, HndlIntSig);
			}
		}
	}
}

int do_sigtrp(
		int tn, /* EM trap number */
		int sn  /* UNIX signal number */
		)
{
	register int old_tn;

	if (sn <= 0 || sn > NSIG)
	{
		einval(WILLSN);
		return (-1);
	}

	if (UNIX_trap(sn))
	{
		einval(WUNIXTR);
		return (-1);
	}

	old_tn = sig_map[sn];
	sig_map[sn] = tn;
	if (tn == -2)
	{ /* reset default for signal sn */
		signal(sn, SIG_DFL);
	}
	else if (tn == -3)
	{ /* ignore signal sn */
		signal(sn, SIG_IGN);
	}
	else if (tn >= 0 && tn <= 252)
	{/* legal tn */
		if (signal(sn, HndlEmSig) == SIG_ERR)
		{
			sig_map[sn] = old_tn;
			return (-1);
		}
	}
	else
	{
		/* illegal trap number */
		einval(WILLTN);
		sig_map[sn] = old_tn; /* restore sig_map */
		return (-1);
	}
	return (old_tn);
}

/**	Execute the trap belonging to the signal that came in during
 *  the last instruction
 */
void trap_signal(void)
{
	register int old_sig = signalled;

	signalled = 0;
	trap(sig_map[old_sig]);
}

PRIVATE void HndlIntSig(int sn)
{
	/* The interpreter got the signal */
	signal(sn, SIG_IGN); /* peace and quiet for close_down() */
	LOG(("@t1 signal %d caught by interpreter", sn));
	message(
			"interpreter received signal %d, which was not caught by the interpreted program",
			sn);
	close_down(1);
}

PRIVATE void HndlEmSig(int sn)
{
	/* The EM machine got the signal */
	signal(sn, HndlIntSig); /* Revert to old situation */
	signalled = sn;
}

