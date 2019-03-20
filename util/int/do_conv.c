/** @file
 * Sources of the "CONVERT" group instructions
 */

/* $Id$ */

#include	"em_abs.h"
#include	"nofloat.h"
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"trap.h"
#include	"text.h"
#include	"fra.h"
#include	"warn.h"
#include	"stack.h"

void DoCII(void)
{
	/* CII -: Convert integer to integer (*) */
	register int newsize = swpop();
	register long s;

	LOG(("@C6 DoCII()"));
	spoilFRA();
	switch ((int)(10 * swpop() + newsize)) {
	case 12:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		npush(spop(1L), 2L);
		return;
	case 14:
		npush(spop(1L), 4L);
		return;
	case 22:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		return;
	case 24:
		npush(spop(2L), 4L);
		return;
	case 42:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		s = spop(4L);
		if (must_test && !(IgnMask&BIT(ECONV))) {
			if (s < I_MINS2 || s > I_MAXS2)
				trap(ECONV);
		}
		npush(s, 2L);
		return;
	case 44:
		return;
	default:
		wtrap(WILLCONV, EILLINS);
	}
}

void DoCUI(void)
{
	/* CUI -: Convert unsigned to integer (*) */
	register int newsize = swpop();
	register unsigned long u;

	LOG(("@C6 DoCUI()"));
	spoilFRA();
	switch ((int)(10 * swpop() + newsize)) {
	case 22:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		u = upop(2L);
		if (must_test && !(IgnMask&BIT(ECONV))) {
			if (u > I_MAXS2)
				trap(ECONV);
		}
		npush((long) u, 2L);
		return;
	case 24:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		npush((long) upop(2L), 4L);
		return;
	case 42:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		u = upop(4L);
		if (must_test && !(IgnMask&BIT(ECONV))) {
			if (u > I_MAXS2)
				trap(ECONV);
		}
		npush((long) u, 2L);
		return;
	case 44:
		u = upop(4L);
		if (must_test && !(IgnMask&BIT(ECONV))) {
			if (u > I_MAXS4)
				trap(ECONV);
		}
		npush((long) u, 4L);
		return;
	default:
		wtrap(WILLCONV, EILLINS);
	}
}

void DoCFI(void)
{
	/* CFI -: Convert floating to integer (*) */
#ifndef	NOFLOAT
	register int newsize = swpop();
	double f;

	LOG(("@C6 DoCFI()"));
	spoilFRA();
	switch ((int)(10 * swpop() + newsize)) {
	case 42:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		f = fpop(4L);
		if (must_test && !(IgnMask&BIT(ECONV))) {
			if (f <= (FL_MINS2 - 1.0) || f > FL_MAXS2)
				trap(ECONV);
		}
		npush((long) f, 2L);
		return;
	case 44:
		f = fpop(4L);
		if (must_test && !(IgnMask&BIT(ECONV))) {
			if (f <= (FL_MINS4 - 1.0) || f > FL_MAXS4)
				trap(ECONV);
		}
		npush((long) f, 4L);
		return;
	case 82:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		f = fpop(8L);
		if (must_test && !(IgnMask&BIT(ECONV))) {
			if (f <= (FL_MINS2 - 1.0) || f > FL_MAXS2)
				trap(ECONV);
		}
		npush((long) f, 2L);
		return;
	case 84:
		f = fpop(8L);
		if (must_test && !(IgnMask&BIT(ECONV))) {
			if (f <= (FL_MINS4 - 1.0) || f > FL_MAXS4)
				trap(ECONV);
		}
		npush((long) f, 4L);
		return;
	default:
		wtrap(WILLCONV, EILLINS);
	}
#else	/* NOFLOAT */
	nofloat();
#endif	/* NOFLOAT */
}

void DoCIF(void)
{
	/* CIF -: Convert integer to floating (*) */
#ifndef	NOFLOAT
	register int newsize = swpop();

	LOG(("@C6 DoCIF()"));
	spoilFRA();
	switch ((int)(10 * swpop() + newsize)) {
	case 24:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		fpush((double) spop(2L), 4L);
		return;
	case 28:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		fpush((double) spop(2L), 8L);
		return;
	case 44:
		fpush((double) spop(4L), 4L);
		return;
	case 48:
		fpush((double) spop(4L), 8L);
		return;
	default:
		wtrap(WILLCONV, EILLINS);
	}
#else	/* NOFLOAT */
	nofloat();
#endif	/* NOFLOAT */
}

void DoCUF(void)
{
	/* CUF -: Convert unsigned to floating (*) */
#ifndef	NOFLOAT
	register int newsize = swpop();
	register unsigned long u;

	LOG(("@C6 DoCUF()"));
	spoilFRA();
	switch ((int)(10 * swpop() + newsize)) {
	case 24:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		fpush((double) upop(2L), 4L);
		return;
	case 28:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		fpush((double) upop(2L), 8L);
		return;
	case 44:
		if ((u = upop(4L)) > I_MAXS4) {
			u -= I_MAXS4;
			u -= 1;
			fpush(((double) u) - (double)(-I_MAXS4-1), 4L);
		}
		else fpush((double) u, 4L);
		return;
	case 48:
		if ((u = upop(4L)) > I_MAXS4) {
			u -= I_MAXS4;
			u -= 1;
			fpush(((double) u) - (double)(-I_MAXS4-1), 8L);
		}
		else fpush((double) u, 8L);
		return;
	default:
		wtrap(WILLCONV, EILLINS);
	}
#else	/* NOFLOAT */
	nofloat();
#endif	/* NOFLOAT */
}

void DoCFF(void)
{
	/* CFF -: Convert floating to floating (*) */
#ifndef	NOFLOAT
	register int newsize = swpop();

	LOG(("@C6 DoCFF()"));
	spoilFRA();
	switch ((int)(10 * swpop() + newsize)) {
	case 44:
		return;
	case 48:
		fpush(fpop(4L), 8L);
		return;
	case 88:
		return;
	case 84:
		fpush(fpop(8L), 4L);
		return;
	default:
		wtrap(WILLCONV, EILLINS);
	}
#else	/* NOFLOAT */
	nofloat();
#endif	/* NOFLOAT */
}

void DoCIU(void)
{
	/* CIU -: Convert integer to unsigned */
	register int newsize = swpop();
	register long u;

	LOG(("@C6 DoCIU()"));
	spoilFRA();
	switch ((int)(10 * swpop() + newsize)) {
	case 22:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		return;
	case 24:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		npush((long) upop(2L), 4L);
		return;
	case 42:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		u = upop(4L);
		npush(u, 2L);
		return;
	case 44:
		return;
	default:
		wtrap(WILLCONV, EILLINS);
	}
}

void DoCUU(void)
{
	/* CUU -: Convert unsigned to unsigned */
	register int newsize = swpop();

	LOG(("@C6 DoCUU()"));
	spoilFRA();
	switch ((int)(10 * swpop() + newsize)) {
	case 22:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		return;
	case 24:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		npush((long) upop(2L), 4L);
		return;
	case 42:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		npush((long) upop(4L), 2L);
		return;
	case 44:
		return;
	default:
		wtrap(WILLCONV, EILLINS);
	}
}

void DoCFU(void)
{
	/* CFU -: Convert floating to unsigned */
#ifndef	NOFLOAT
	register int newsize = swpop();
	double f;

	LOG(("@C6 DoCFU()"));
	spoilFRA();
	switch ((int)(10 * swpop() + newsize)) {
	case 42:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		f = fpop(4L);
		npush((unsigned long) f, 2L);
		return;
	case 44:
		f = fpop(4L);
		npush((unsigned long) f, 4L);
		return;
	case 82:
		if (wsize == 4) {
			wtrap(WILLCONV, EILLINS);
		}
		f = fpop(8L);
		npush((unsigned long) f, 2L);
		return;
	case 84:
		f = fpop(8L);
		npush((unsigned long) f, 4L);
		return;
	default:
		wtrap(WILLCONV, EILLINS);
	}
#else	/* NOFLOAT */
	nofloat();
#endif	/* NOFLOAT */
}
