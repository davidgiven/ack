.define .dvi8
.sect .text
.sect .rom
.sect .data
.sect .bss

yh=8
yl=12
xh=16
xl=20
	! .dvi8 yields d0:d1 = quotient from x / y

.sect .text
.dvi8:
	move.l	d3, -(sp)	! preserve caller's d3
	clr.l	d2		! d2 = 0, non-negative result
	move.l	(xh, sp), d0	! d0 for .divrem8
	bge	1f
	move.l	#1, d2		! d2 = 1, negative result
	neg.l	(xl, sp)
	negx.l	d0		! x = absolute value
1:	move.l	(yh, sp), d1	! d1 for .divrem8
	bge	1f
	bchg	#0, d2		! flip sign of result
	neg.l	(yl, sp)
	negx.l	d1		! y = absolute value
1:	move.l	d2, -(sp)
	jsr	(.divrem8)
	move.l	(sp)+, d2
	beq	1f		! branch unless result < 0
	neg.l	d1
	negx.l	d0		! negate quotient d0:d1
1:	move.l	(sp)+, d3
	rtd	#16
