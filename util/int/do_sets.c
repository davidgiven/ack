/** @file
 *  Sources of the "SETS" group instructions
 */

/* $Id$ */

#include	"em_abs.h"
#include	"global.h"
#include	"log.h"
#include	"trap.h"
#include	"mem.h"
#include	"text.h"
#include	"fra.h"

PRIVATE void bit_test(size), create_set(size);

/** INN w: Bit test on w byte set (bit number on top of stack) */
void DoINN(register size l)
{
	LOG(("@Y6 DoINN(%ld)", l));
	spoilFRA();
	bit_test(arg_w(l));
}

/** SET w: Create singleton w byte set with bit n on (n is top of stack) */
void DoSET(register size l)
{
	LOG(("@Y6 DoSET(%ld)", l));
	spoilFRA();
	create_set(arg_w(l));
}

/** Bit testing. Tests whether the bit with number to be found
 *  on TOS is on in 'w'-byte set.
 *  ON --> push 1 on stack.
 *  OFF -> push 0 on stack.
 **/
PRIVATE void bit_test(size w)
{
	register int bitno =
		(int) swpop();	/* bitno on TOS */
	register char test_byte = (char) 0;/* default value to be tested */
	register int wordoff = bitno / 8;
	register int bitoff = bitno % 8; 

	if (bitoff < 0) bitoff += 8;

	if (must_test && !(IgnMask&BIT(ESET))) {
		/* Only w*8 bits CAN be tested */
		if (wordoff >= w) {
			trap(ESET);
		}
	}
	test_byte = stack_loc(SP + wordoff);
	st_dec(w);
	wpush((long)((test_byte & BIT(bitoff)) ? 1 : 0));
}

/** Set creation. Creates a singleton 'w'-byte set with as
 *  singleton member, the bit with number on TOS.
 *  The w bytes constituting the set are
 *  pushed on the stack.
 **/
PRIVATE void create_set(size w)
{
	register int bitno = (int) swpop();
	register size nbytes = w;
	register int wordoff = bitno / 8;
	register int bitoff = bitno % 8;

	if (bitoff < 0) bitoff += 8;

	st_inc(nbytes);
	while (--nbytes >= 0) {
		st_stn(SP + nbytes, 0L, 1L);
	}

	if (must_test && !(IgnMask&BIT(ESET))) {
		if (wordoff >= w) {
			trap(ESET);
		}
	}
	st_stn(SP + wordoff, (long)BIT(bitoff), 1L);
}

