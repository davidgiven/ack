.define .dvu8
.sect .text
.sect .rom
.sect .data
.sect .bss

yh=8
xh=16
	! .dvu8 yields d0:d1 = quotient from x / y

.sect .text
.dvu8:
	move.l	d3, -(sp)	! preserve caller's d3
	move.l	(xh, sp), d0
	move.l	(yh, sp), d1
	sub.l	#4, sp
	jsr	(.divrem8)
	add.l	#4, sp
	move.l	(sp)+, d3
	rtd	#16
