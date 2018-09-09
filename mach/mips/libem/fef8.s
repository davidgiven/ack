#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Split a double-precision float into fraction and exponent, like
 * frexp(3) in C, http://en.cppreference.com/w/c/numeric/math/frexp
 *
 * Stack: ( double -- fraction exponent )
 */

#define DBL_EXPBITS	11
#define DBL_FRACHBITS	20
#define DBL_FRACLBITS	32
#define DBL_FRACBITS	52
#define DBL_EXP_INFNAN	2047

#define DBL_EXP_BIAS 1023

.sect .text
.define .fef8
.fef8:
	lw r4, 0(sp)			! r4 = low word (bits 0..31)
	lw r5, 4(sp)			! r5 = high word (bits 32..63)

	! IEEE double = sign * 1.fraction * 2**(exponent - 1023)
	!   sign  exponent  fraction
	!   31     30..19     18..0, 31..0
	!
	! IEEE exponent = 1022 in [0.5, 1) or (-1, -0.5].

	ext r7, r5, DBL_FRACHBITS, DBL_EXPBITS ! r7 = IEEE exponent
	beq r7, zero, zeroexp   ! this number is zero or denormalised, treat specially
	nop

	li at, DBL_EXP_INFNAN
	beq r7, at, return      ! just return if infinity or NaN
	nop

	addiu r7, r7, -[DBL_EXP_BIAS-1] ! undo exponent bias
	li at, DBL_EXP_BIAS-1
	ins r5, at, DBL_FRACHBITS, DBL_EXPBITS ! replace exponent
return:
	addiu sp, sp, -4        ! returning one more quad than we got
	sw r5, 8(sp)
	sw r6, 4(sp)
	sw r7, 0(sp)
	jr ra
	nop

	/* We received a denormalised number or zero. */
zeroexp:
	/* TODO: we just assume that the number is zero here. */
	mov r5, zero
	mov r6, zero
	mov r7, zero
	b return
	nop
