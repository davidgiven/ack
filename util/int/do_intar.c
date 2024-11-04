/** @file
 *  Sources of the "INTEGER ARITHMETIC" group instructions
 */

/* $Id$ */

#include "em_abs.h"
#include "logging.h"
#include "global.h"
#include "log.h"
#include "mem.h"
#include "trap.h"
#include "warn.h"
#include "text.h"
#include "fra.h"
#include "switch.h"

static long adi(long, long, size), sbi(long, long, size), dvi(long, long, size);
static long mli(long, long, size), rmi(long, long), ngi(long, size);
static long sli(long, long, size), sri(long, long, size);

/** ADI w: Addition (*) */
void DoADI(size l)
{
	long t = spop(arg_wi(l));

	LOG(("@I6 DoADI(%ld)", l));
	spoilFRA();
	npush(adi(spop(l), t, l), l);
}

/** SBI w: Subtraction (*) */
void DoSBI(size l)
{
	long t = spop(arg_wi(l));

	LOG(("@I6 DoSBI(%ld)", l));
	spoilFRA();
	npush(sbi(spop(l), t, l), l);
}

/** MLI w: Multiplication (*) */
void DoMLI(size l)
{
	long t = spop(arg_wi(l));

	LOG(("@I6 DoMLI(%ld)", l));
	spoilFRA();
	npush(mli(spop(l), t, l), l);
}

/** DVI w: Division (*) */
void DoDVI(size l)
{
	long t = spop(arg_wi(l));

	LOG(("@I6 DoDVI(%ld)", l));
	spoilFRA();
	npush(dvi(spop(l), t, l), l);
}

/** RMI w: Remainder (*) */
void DoRMI(size l)
{
	long t = spop(arg_wi(l));

	LOG(("@I6 DoRMI(%ld)", l));
	spoilFRA();
	npush(rmi(spop(l), t), l);
}

/** NGI w: Negate (two's complement) (*) */
void DoNGI(size l)
{
	LOG(("@I6 DoNGI(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(ngi(spop(l), l), l);
}

/** SLI w: Shift left (*) */
void DoSLI(size l)
{
	long t = swpop();

	LOG(("@I6 DoSLI(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(sli(spop(l), t, l), l);
}

/** SRI w: Shift right (*) */
void DoSRI(size l)
{
	long t = swpop();

	LOG(("@I6 DoSRI(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(sri(spop(l), t, l), l);
}

#define i_maxs(n) ((n == 2) ? I_MAXS2 : I_MAXS4)
#define i_mins(n) ((n == 2) ? I_MINS2 : I_MINS4)

/** Returns "w1" + "w2". */
static long adi(long w1, long w2, size nbytes)
{
	if (must_test && !(IgnMask & BIT(EIOVFL)))
	{
		if (w1 > 0 && w2 > 0)
		{
			if (i_maxs(nbytes) - w1 < w2)
				trap(EIOVFL);
		}
		else if (w1 < 0 && w2 < 0)
		{
			if (i_mins(nbytes) - w1 > w2)
				trap(EIOVFL);
		}
	}
	return (w1 + w2);
}

/** Returns "w1" - "w2" */
static long sbi(long w1, long w2, size nbytes)
{
	if (must_test && !(IgnMask & BIT(EIOVFL)))
	{
		if (w2 < 0 && w1 > 0)
		{
			if (i_maxs(nbytes) + w2 < w1)
				trap(EIOVFL);
		}
		else if (w2 > 0 && w1 < 0)
		{
			if (i_mins(nbytes) + w2 > w1)
			{
				trap(EIOVFL);
			}
		}
	}
	return (w1 - w2);
}

#define labs(w) ((w < 0) ? (-w) : w)

/** Returns "w1" * "w2" */
static long mli(long w1, long w2, size nbytes)
{
	if (w1 == 0 || w2 == 0)
		return (0L);

	if (must_test && !(IgnMask & BIT(EIOVFL)))
	{
		if ((w1 > 0 && w2 > 0) || (w2 < 0 && w1 < 0))
		{
			if (w1 == i_mins(nbytes) || w2 == i_mins(nbytes)
			    || (i_maxs(nbytes) / labs(w1)) < labs(w2))
			{
				trap(EIOVFL);
			}
		}
		else if (w1 > 0)
		{
			if (i_mins(nbytes) / w1 > w2)
				trap(EIOVFL);
		}
		else if (i_mins(nbytes) / w2 > w1)
		{
			trap(EIOVFL);
		}
	}
	return (w1 * w2);
}

static long dvi(long w1, long w2, size nbytes)
{
	if (w2 == 0)
	{
		if (!(IgnMask & BIT(EIDIVZ)))
		{
			trap(EIDIVZ);
		}
		else
			return (0L);
	}

	/* Check for division overflow. */
	if ((w1 == i_mins(nbytes)) && (w2 == -1))
	{
		if (must_test && !(IgnMask & BIT(EIOVFL)))
		{
			trap(EIOVFL);
		}
		else
			return i_mins(nbytes);
	}

	return (w1 / w2);
}

static long rmi(long w1, long w2)
{
	if (w2 == 0)
	{
		if (!(IgnMask & BIT(EIDIVZ)))
		{
			trap(EIDIVZ);
		}
		else
			return (0L);
	}
	return (w1 % w2);
}

static long ngi(long w1, size nbytes)
{
	if (must_test && !(IgnMask & BIT(EIOVFL)))
	{
		if (w1 == i_mins(nbytes))
		{
			trap(EIOVFL);
		}
	}
	return (-w1);
}

/** "w1" << "w2" */
static long sli(long w1, long w2, size nbytes)
{
	if (must_test)
	{
#ifdef LOGGING
		/* check shift distance */
		if (w2 < 0)
		{
			warning(WSHNEG);
			w2 = 0;
		}
		if (w2 >= nbytes * 8)
		{
			warning(WSHLARGE);
			w2 = nbytes * 8 - 1;
		}
#endif /* LOGGING */

		if (!(IgnMask & BIT(EIOVFL)))
		{
			/* check overflow */

			/* If the value is positive, then check, this is taken
			 * from rule INT32-C of SEI website.
			 */
			if ((w1 >= 0) && (w1 > (i_maxs(nbytes) >> w2)))
			{
				trap(EIOVFL);
			}

			if ((w1 < 0) && (w1 < (i_mins(nbytes) >> w2)))
			{
				trap(EIOVFL);
			}
		}
	}

	/* calculate result */
	return (w1 << w2);
}

/*ARGSUSED*/
static long sri(long w1, long w2, size nbytes) /* w1 >> w2 */
{
#ifdef LOGGING
	if (must_test)
	{
		/* check shift distance */
		if (w2 < 0)
		{
			warning(WSHNEG);
			w2 = 0;
		}
		if (w2 >= nbytes * 8)
		{
			warning(WSHLARGE);
			w2 = nbytes * 8 - 1;
		}
	}
#endif /* LOGGING */

	/* calculate result */
	return (w1 >> w2);
}
