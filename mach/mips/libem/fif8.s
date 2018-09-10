#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Multiplies two double-precision floats, then splits the product into
 *  fraction and integer, both as floats, like modf(3) in C,
 *  http://en.cppreference.com/w/c/numeric/math/modf
 *
 * Stack: ( a b -- fraction integer )
 */

.sect .text
.define .fif8
.fif8:
	ldc1 f0, 8(sp)          ! f0 = a
	ldc1 f2, 0(sp)          ! f2 = b
	mul.d f0, f0, f2        ! f0 = a * b
	abs.d f2, f0            ! f2 = abs(f0)

	li at, ha16[max_power_of_two]
	ldc1 f4, lo16[max_power_of_two] (at) ! f4 = max power of two

	mov.d f6, f2            ! we're going to assemble the integer part in f6
	c.lt.d 0, f4, f2        ! if absolute value too big, it must be integral
	bc1t 0, return
	nop

	! Crudely strip off the fractional part.

	add.d f6, f2, f4        ! f6 = absolute value + max power of two
	sub.d f6, f6, f4        ! f6 -= max_power_of_two

	! The above might round, so correct that.

	li at, ha16[one]
	ldc1 f8, lo16[one] (at)   ! f8 = 1.0
1:
	c.le.d 0, f6, f2        ! if result <= absolute value, stop
	bc1t 0, 2f
	nop

	sub.d f6, f6, f8        ! result -= 1.0
	b 1b
	nop
2:

	! Correct the sign of the result.

	mtc1 zero, f8
	mthc1 zero, f8          ! f8 = 0.0
	c.lt.d 0, f0, f8        ! if original value was negative
	bc1f 0, 1f
	nop
	neg.d f6, f6            ! negate the result
1:

return:
	sdc1 f6, 0(sp)          ! store integer part
	sub.d f6, f0, f6        ! calculate fractional part
	sdc1 f6, 8(sp)          ! store fractional part
	jr ra
	nop

! doubles >= MAXPOWTWO are already integers
.sect .rom
max_power_of_two:
	.dataf8 4.503599627370496000E+15

one:
	.dataf8 1.0
