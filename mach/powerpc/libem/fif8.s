#include "powerpc.h"

.sect .text

! Multiplies two double-precision floats, then splits the product into
! integer and fraction, like modf(3) in C.  On entry:
!  f1 = float
!  f2 = other float
! Yields:
!  f1 = fraction
!  f2 = integer
! Kills: cr0 f1 f2 r3 r4 r5 r6

.define .fif8
.fif8:
	fmul f1, f1, f2
	stfdu f1, -8(sp)		! push f1 = product
	lwz r3, 0(sp)			! r3 = high word
	lwz r4, 4(sp)			! r4 = low word

	! IEEE double-precision format:
	!   sign  exponent  fraction
	!   0     1..11     12..63
	! Subtract 1023 from the IEEE exponent.  If the result is from
	! 0 to 51, then the IEEE fraction has that many integer bits.
	! (IEEE has an implicit 1 before its fraction.  If the IEEE
	! fraction has 0 integer bits, we still have an integer.)
	rlwinm r5, r3, 12, 21, 31	! r5 = IEEE exponent
	addic. r5, r5, -1023		! r5 = nr of integer bits
	bc IFTRUE, LT, no_int
	cmpi cr0, 0, r5, 21
	bc IFTRUE, LT, small_int
	cmpi cr0, 0, r5, 52
	bc IFTRUE, LT, big_int

	! f1 is an integer without fraction.  Jump to calculate
	! fraction f1 = f2 - f1.  It will be zero (or perhaps NaN).
	fmr f2, f1
	b subtract

no_int:
	! f1 is a fraction without integer.
	fsub f2, f1, f1			! integer = zero
	b done

small_int:
	! f1 has r5 = 0 to 20 integer bits in the IEEE fraction.
	! High word has 20 - r5 fraction bits.
	addi r6, r0, 20
	subf r6, r5, r6
	srw r3, r3, r6
	addi r4, r0, 0			! clear low word
	slw r3, r3, r6			! clear fraction in high word
	b move_int

big_int:
	! f1 has r5 = 21 to 51 to integer bits.
	! Low word has 52 - r5 fraction bits.
	addi r6, r0, 52
	subf r6, r5, r6
	srw r4, r4, r6
	slw r4, r4, r6			! clear fraction in low word
move_int:
	stw r3, 0(sp)
	stw r4, 4(sp)
	lfd f2, 0(sp)			! f2 = integer
subtract:
	fsub f1, f1, f2			! fraction = value - integer
done:
	addi sp, sp, 8			! restore stack pointer
	bclr ALWAYS, 0, 0
