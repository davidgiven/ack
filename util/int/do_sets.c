/*
 * Sources of the "SETS" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
#include	"global.h"
#include	"log.h"
#include	"trap.h"
#include	"mem.h"
#include	"text.h"
#include	"fra.h"

PRIVATE bit_test(), create_set();

DoINNl2(arg)
	size arg;
{
	/* INN w: Bit test on w byte set (bit number on top of stack) */
	register size l = (L_arg_2() * arg);

	LOG(("@Y6 DoINNl2(%ld)", l));
	spoilFRA();
	bit_test(arg_w(l));
}

DoINNs(hob, wfac)
	long hob;
	size wfac;
{
	/* INN w: Bit test on w byte set (bit number on top of stack) */
	register size l = (S_arg(hob) * wfac);

	LOG(("@Y6 DoINNs(%ld)", l));
	spoilFRA();
	bit_test(arg_w(l));
}

DoINNz()
{
	/* INN w: Bit test on w byte set (bit number on top of stack) */
	register size l = uwpop();

	LOG(("@Y6 DoINNz(%ld)", l));
	spoilFRA();
	bit_test(arg_w(l));
}

DoSETl2(arg)
	size arg;
{
	/* SET w: Create singleton w byte set with bit n on (n is top of stack) */
	register size l = (L_arg_2() * arg);

	LOG(("@Y6 DoSETl2(%ld)", l));
	spoilFRA();
	create_set(arg_w(l));
}

DoSETs(hob, wfac)
	long hob;
	size wfac;
{
	/* SET w: Create singleton w byte set with bit n on (n is top of stack) */
	register size l = (S_arg(hob) * wfac);

	LOG(("@Y6 DoSETs(%ld)", l));
	spoilFRA();
	create_set(arg_w(l));
}

DoSETz()
{
	/* SET w: Create singleton w byte set with bit n on (n is top of stack) */
	register size l = uwpop();

	LOG(("@Y6 DoSETz(%ld)", l));
	spoilFRA();
	create_set(arg_w(l));
}

/********************************************************
 *		bit testing				*
 *							*
 *	Tests whether the bit with number to be found	*
 *	on TOS is on in 'w'-byte set.			*
 *	ON --> push 1 on stack.				*
 *	OFF -> push 0 on stack.				*
 ********************************************************/

PRIVATE bit_test(w)
	size w;
{
	register int bitno =
		(int) swpop();	/* bitno on TOS */
	register char test_byte = (char) 0;/* default value to be tested */

	if (must_test && !(IgnMask&BIT(ESET))) {
		/* Only w<<3 bytes CAN be tested */
		if (bitno > (int) ((w << 3) - 1)) {
			trap(ESET);
		}
	}
	test_byte = stack_loc(SP + (bitno >> 3));
	st_dec(w);
	wpush((long)((test_byte & BIT(bitno & 7)) ? 1 : 0));
}

/********************************************************
 *		set creation				*
 *							*
 *	Creates a singleton 'w'-byte set with as	*
 *	singleton member, the bit with number on	*
 *	TOS. The w bytes constituting the set are	*
 *	pushed on the stack.				*
 ********************************************************/

PRIVATE create_set(w)
	size w;
{
	register int bitno = (int) swpop();
	register size nbytes = w;

	st_inc(nbytes);
	while (--nbytes >= 0) {
		st_stn(SP + nbytes, 0L, 1L);
	}

	if (must_test && !(IgnMask&BIT(ESET))) {
		if (bitno > (int) ((w << 3) - 1)) {
			trap(ESET);
		}
	}
	st_stn(SP + (bitno >> 3), (long)BIT(bitno & 7), 1L);
}

