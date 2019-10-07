.define .rmi8
.sect .text
.sect .rom
.sect .data
.sect .bss

yh=8
yl=12
xh=16
xl=20
	! .rmi8 yields d0:d1 = remainder from x / y

.sect .text
.rmi8:
	move.l	d3, -(sp)	! preserve caller's d3
	clr.l	d2		! d2 = 0, non-negative result
	move.l	(xh, sp), d0	! d0 for .divrem8
	bge	1f
	move.l	#1, d2		! d2 = 1, negative result
	neg.l	(xl, sp)
	negx.l	d0		! x = absolute value
1:	move.l	(yh, sp), d1	! d1 for .divrem8
	bge	1f
	neg.l	(yl, sp)
	negx.l	d1		! y = absolute value
1:	move.l	d2, -(sp)
	jsr	(.divrem8)
	move.l	(sp)+, d0
	beq	1f		! branch unless result < 0
	neg.l	d3
	negx.l	d2		! negate quotient d3:d2
1:	move.l	d3, d1
	move.l	d2, d0
	move.l	(sp)+, d3
	rtd	#16
