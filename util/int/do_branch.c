/*
 * Sources of the "BRANCH" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"trap.h"
#include	"text.h"
#include	"fra.h"
#include	"warn.h"

/*	Note that in the EM assembly language brach instructions have
	lables as their arguments, where in the EM machine language they
	have (relative) offsets as parameters.  This is not described in the
	EM manual but follows from the Pascal interpreter.
*/

#define	do_jump(j)	{ newPC(PC + (j)); }

DoBRAl2(arg)
	long arg;
{
	/* BRA b: Branch unconditionally to label b */
	register long jump = (L_arg_2() * arg);

	LOG(("@B6 DoBRAl2(%ld)", jump));
	do_jump(arg_c(jump));
}

DoBRAl4(arg)
	long arg;
{
	/* BRA b: Branch unconditionally to label b */
	register long jump = (L_arg_4() * arg);

	LOG(("@B6 DoBRAl4(%ld)", jump));
	do_jump(arg_c(jump));
}

DoBRAs(hob, wfac)
	long hob;
	size wfac;
{
	/* BRA b: Branch unconditionally to label b */
	register long jump = (S_arg(hob) * wfac);

	LOG(("@B6 DoBRAs(%ld)", jump));
	do_jump(arg_c(jump));
}

DoBLTl2(arg)
	long arg;
{
	/* BLT b: Branch less (pop 2 words, branch if top > second) */
	register long jump = (L_arg_2() * arg);
	register long t = wpop();

	LOG(("@B6 DoBLTl2(%ld)", jump));
	spoilFRA();
	if (wpop() < t)
		do_jump(arg_c(jump));
}

DoBLTl4(arg)
	long arg;
{
	/* BLT b: Branch less (pop 2 words, branch if top > second) */
	register long jump = (L_arg_4() * arg);
	register long t = wpop();

	LOG(("@B6 DoBLTl4(%ld)", jump));
	spoilFRA();
	if (wpop() < t)
		do_jump(arg_c(jump));
}

DoBLTs(hob, wfac)
	long hob;
	size wfac;
{
	/* BLT b: Branch less (pop 2 words, branch if top > second) */
	register long jump = (S_arg(hob) * wfac);
	register long t = wpop();

	LOG(("@B6 DoBLTs(%ld)", jump));
	spoilFRA();
	if (wpop() < t)
		do_jump(arg_c(jump));
}

DoBLEl2(arg)
	long arg;
{
	/* BLE b: Branch less or equal */
	register long jump = (L_arg_2() * arg);
	register long t = wpop();

	LOG(("@B6 DoBLEl2(%ld)", jump));
	spoilFRA();
	if (wpop() <= t)
		do_jump(arg_c(jump));
}

DoBLEl4(arg)
	long arg;
{
	/* BLE b: Branch less or equal */
	register long jump = (L_arg_4() * arg);
	register long t = wpop();

	LOG(("@B6 DoBLEl4(%ld)", jump));
	spoilFRA();
	if (wpop() <= t)
		do_jump(arg_c(jump));
}

DoBLEs(hob, wfac)
	long hob;
	size wfac;
{
	/* BLE b: Branch less or equal */
	register long jump = (S_arg(hob) * wfac);
	register long t = wpop();

	LOG(("@B6 DoBLEs(%ld)", jump));
	spoilFRA();
	if (wpop() <= t)
		do_jump(arg_c(jump));
}

DoBEQl2(arg)
	long arg;
{
	/* BEQ b: Branch equal */
	register long jump = (L_arg_2() * arg);
	register long t = wpop();

	LOG(("@B6 DoBEQl2(%ld)", jump));
	spoilFRA();
	if (t == wpop())
		do_jump(arg_c(jump));
}

DoBEQl4(arg)
	long arg;
{
	/* BEQ b: Branch equal */
	register long jump = (L_arg_4() * arg);
	register long t = wpop();

	LOG(("@B6 DoBEQl4(%ld)", jump));
	spoilFRA();
	if (t == wpop())
		do_jump(arg_c(jump));
}

DoBEQs(hob, wfac)
	long hob;
	size wfac;
{
	/* BEQ b: Branch equal */
	register long jump = (S_arg(hob) * wfac);
	register long t = wpop();

	LOG(("@B6 DoBEQs(%ld)", jump));
	spoilFRA();
	if (t == wpop())
		do_jump(arg_c(jump));
}

DoBNEl2(arg)
	long arg;
{
	/* BNE b: Branch not equal */
	register long jump = (L_arg_2() * arg);
	register long t = wpop();

	LOG(("@B6 DoBNEl2(%ld)", jump));
	spoilFRA();
	if (t != wpop())
		do_jump(arg_c(jump));
}

DoBNEl4(arg)
	long arg;
{
	/* BNE b: Branch not equal */
	register long jump = (L_arg_4() * arg);
	register long t = wpop();

	LOG(("@B6 DoBNEl4(%ld)", jump));
	spoilFRA();
	if (t != wpop())
		do_jump(arg_c(jump));
}

DoBNEs(hob, wfac)
	long hob;
	size wfac;
{
	/* BNE b: Branch not equal */
	register long jump = (S_arg(hob) * wfac);
	register long t = wpop();

	LOG(("@B6 DoBNEs(%ld)", jump));
	spoilFRA();
	if (t != wpop())
		do_jump(arg_c(jump));
}

DoBGEl2(arg)
	long arg;
{
	/* BGE b: Branch greater or equal */
	register long jump = (L_arg_2() * arg);
	register long t = wpop();

	LOG(("@B6 DoBGEl2(%ld)", jump));
	spoilFRA();
	if (wpop() >= t)
		do_jump(arg_c(jump));
}

DoBGEl4(arg)
	long arg;
{
	/* BGE b: Branch greater or equal */
	register long jump = (L_arg_4() * arg);
	register long t = wpop();

	LOG(("@B6 DoBGEl4(%ld)", jump));
	spoilFRA();
	if (wpop() >= t)
		do_jump(arg_c(jump));
}

DoBGEs(hob, wfac)
	long hob;
	size wfac;
{
	/* BGE b: Branch greater or equal */
	register long jump = (S_arg(hob) * wfac);
	register long t = wpop();

	LOG(("@B6 DoBGEs(%ld)", jump));
	spoilFRA();
	if (wpop() >= t)
		do_jump(arg_c(jump));
}

DoBGTl2(arg)
	long arg;
{
	/* BGT b: Branch greater */
	register long jump = (L_arg_2() * arg);
	register long t = wpop();

	LOG(("@B6 DoBGTl2(%ld)", jump));
	spoilFRA();
	if (wpop() > t)
		do_jump(arg_c(jump));
}

DoBGTl4(arg)
	long arg;
{
	/* BGT b: Branch greater */
	register long jump = (L_arg_4() * arg);
	register long t = wpop();

	LOG(("@B6 DoBGTl4(%ld)", jump));
	spoilFRA();
	if (wpop() > t)
		do_jump(arg_c(jump));
}

DoBGTs(hob, wfac)
	long hob;
	size wfac;
{
	/* BGT b: Branch greater */
	register long jump = (S_arg(hob) * wfac);
	register long t = wpop();

	LOG(("@B6 DoBGTs(%ld)", jump));
	spoilFRA();
	if (wpop() > t)
		do_jump(arg_c(jump));
}

DoZLTl2(arg)
	long arg;
{
	/* ZLT b: Branch less than zero (pop 1 word, branch negative) */
	register long jump = (L_arg_2() * arg);

	LOG(("@B6 DoZLTl2(%ld)", jump));
	spoilFRA();
	if (wpop() < 0)
		do_jump(arg_c(jump));
}

DoZLTl4(arg)
	long arg;
{
	/* ZLT b: Branch less than zero (pop 1 word, branch negative) */
	register long jump = (L_arg_4() * arg);

	LOG(("@B6 DoZLTl4(%ld)", jump));
	spoilFRA();
	if (wpop() < 0)
		do_jump(arg_c(jump));
}

DoZLTs(hob, wfac)
	long hob;
	size wfac;
{
	/* ZLT b: Branch less than zero (pop 1 word, branch negative) */
	register long jump = (S_arg(hob) * wfac);

	LOG(("@B6 DoZLTs(%ld)", jump));
	spoilFRA();
	if (wpop() < 0)
		do_jump(arg_c(jump));
}

DoZLEl2(arg)
	long arg;
{
	/* ZLE b: Branch less or equal to zero */
	register long jump = (L_arg_2() * arg);

	LOG(("@B6 DoZLEl2(%ld)", jump));
	spoilFRA();
	if (wpop() <= 0)
		do_jump(arg_c(jump));
}

DoZLEl4(arg)
	long arg;
{
	/* ZLE b: Branch less or equal to zero */
	register long jump = (L_arg_4() * arg);

	LOG(("@B6 DoZLEl4(%ld)", jump));
	spoilFRA();
	if (wpop() <= 0)
		do_jump(arg_c(jump));
}

DoZLEs(hob, wfac)
	long hob;
	size wfac;
{
	/* ZLE b: Branch less or equal to zero */
	register long jump = (S_arg(hob) * wfac);

	LOG(("@B6 DoZLEs(%ld)", jump));
	spoilFRA();
	if (wpop() <= 0)
		do_jump(arg_c(jump));
}

DoZEQl2(arg)
	long arg;
{
	/* ZEQ b: Branch equal zero */
	register long jump = (L_arg_2() * arg);

	LOG(("@B6 DoZEQl2(%ld)", jump));
	spoilFRA();
	if (wpop() == 0)
		do_jump(arg_c(jump));
}

DoZEQl4(arg)
	long arg;
{
	/* ZEQ b: Branch equal zero */
	register long jump = (L_arg_4() * arg);

	LOG(("@B6 DoZEQl4(%ld)", jump));
	spoilFRA();
	if (wpop() == 0)
		do_jump(arg_c(jump));
}

DoZEQs(hob, wfac)
	long hob;
	size wfac;
{
	/* ZEQ b: Branch equal zero */
	register long jump = (S_arg(hob) * wfac);

	LOG(("@B6 DoZEQs(%ld)", jump));
	spoilFRA();
	if (wpop() == 0)
		do_jump(arg_c(jump));
}

DoZNEl2(arg)
	long arg;
{
	/* ZNE b: Branch not zero */
	register long jump = (L_arg_2() * arg);

	LOG(("@B6 DoZNEl2(%ld)", jump));
	spoilFRA();
	if (wpop() != 0)
		do_jump(arg_c(jump));
}

DoZNEl4(arg)
	long arg;
{
	/* ZNE b: Branch not zero */
	register long jump = (L_arg_4() * arg);

	LOG(("@B6 DoZNEl4(%ld)", jump));
	spoilFRA();
	if (wpop() != 0)
		do_jump(arg_c(jump));
}

DoZNEs(hob, wfac)
	long hob;
	size wfac;
{
	/* ZNE b: Branch not zero */
	register long jump = (S_arg(hob) * wfac);

	LOG(("@B6 DoZNEs(%ld)", jump));
	spoilFRA();
	if (wpop() != 0)
		do_jump(arg_c(jump));
}

DoZGEl2(arg)
	long arg;
{
	/* ZGE b: Branch greater or equal zero */
	register long jump = (L_arg_2() * arg);

	LOG(("@B6 DoZGEl2(%ld)", jump));
	spoilFRA();
	if (wpop() >= 0)
		do_jump(arg_c(jump));
}

DoZGEl4(arg)
	long arg;
{
	/* ZGE b: Branch greater or equal zero */
	register long jump = (L_arg_4() * arg);

	LOG(("@B6 DoZGEl4(%ld)", jump));
	spoilFRA();
	if (wpop() >= 0)
		do_jump(arg_c(jump));
}

DoZGEs(hob, wfac)
	long hob;
	size wfac;
{
	/* ZGE b: Branch greater or equal zero */
	register long jump = (S_arg(hob) * wfac);

	LOG(("@B6 DoZGEs(%ld)", jump));
	spoilFRA();
	if (wpop() >= 0)
		do_jump(arg_c(jump));
}

DoZGTl2(arg)
	long arg;
{
	/* ZGT b: Branch greater than zero */
	register long jump = (L_arg_2() * arg);

	LOG(("@B6 DoZGTl2(%ld)", jump));
	spoilFRA();
	if (wpop() > 0)
		do_jump(arg_c(jump));
}

DoZGTl4(arg)
	long arg;
{
	/* ZGT b: Branch greater than zero */
	register long jump = (L_arg_4() * arg);

	LOG(("@B6 DoZGTl4(%ld)", jump));
	spoilFRA();
	if (wpop() > 0)
		do_jump(arg_c(jump));
}

DoZGTs(hob, wfac)
	long hob;
	size wfac;
{
	/* ZGT b: Branch greater than zero */
	register long jump = (S_arg(hob) * wfac);

	LOG(("@B6 DoZGTs(%ld)", jump));
	spoilFRA();
	if (wpop() > 0)
		do_jump(arg_c(jump));
}
