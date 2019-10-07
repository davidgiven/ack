.define .divrem8
.sect .text
.sect .rom
.sect .data
.sect .bss

yh=16
yl=20
xh=24
xl=28
	! This private sub for .dvi8, .dvu8, .rmi8, .rmu8
	! does unsigned division of x = xh:xl by y = yh:yl,
	! yields d0:d1 = quotient, d2:d3 = remainder.

.sect .text
.divrem8:
	! Caller must set d0, d1 like so:
	! mov.l (xh, sp), d0
	! mov.l (yh, sp), d1
	tst.l	d1
	bne	1f		! branch if y >= 2**32

	! y = yl, so x / y = xh:xl / yl = qh:0 + rh:xl / yl
	! where qh, rh are quotient, remainder from xh / yl.
	move.l	(xl, sp), d1
	move.l	(yl, sp), d2
	clr.l	d3		! d3:d0 = xh
	divu.l	d2, d3:d0	! d0 =  0:xh / yl, d3 = rh
	divu.l	d2, d3:d1	! d1 = rh:xl / yl, so d0:d1 = x / y
	clr.l	d2		! remainder in d2:d3
	rts

1:	! Here y >= 2**32.
	move.l	d0, a0		! save xh
	move.l	d1, a1		! save yh
	move.l	d7, a2		! save caller's d7

	! Find y >> right in [2**31, 2**32).
	move.l	(yl, sp), d2
	bfffo	d1[0:32], d3	! find highest set bit in yh
	lsl.l	d3, d1		! shift yh left
	bset	#5, d3
	neg.l	d3		! right = (32 - left) modulo 64
	lsr.l	d3, d2		! shift yl right
	or.l	d1, d2		! d2 = y >> right

	! Estimate x / y as q = (x / (y >> right)) >> right.
	move.l	(xl, sp), d1
	clr.l	d7
	divu.l	d2, d7:d0
	divu.l	d2, d7:d1	! d0:d1 = x / (y >> right)
	lsr.l	d3, d1
	bset	#5, d3
	neg.l	d3
	lsl.l	d3, d0
	or.l	d0, d1		! d1 = q

	! Calculate the remainder x - y * q.  If the subtraction
	! overflows, then the correct quotient is q - 1, else it is q.
	move.l	a1, d3		! yh
	mulu.l	d1, d3		! yh * q
	move.l	(yl, sp), d7
	mulu.l	d1, d0:d7	! yl * q
	add.l	d3, d0		! d0:d7 = y * q
	move.l	(xl, sp), d3
	move.l	a0, d2		! d2:d3 = x
	sub.l	d7, d3
	subx.l	d0, d2		! d2:d3 = x - y * q
	bcc	1f		! branch unless subtraction overflowed
	sub.l	#1, d1		! fix quotient
	move.l	a1, d7		! yh
	add.l	(yl, sp), d3
	addx.l	d7, d2		! fix remainder
1:	clr.l	d0		! d0:d1 = quotient
	move.l	a2, d7		! restore caller's d7
	rts
