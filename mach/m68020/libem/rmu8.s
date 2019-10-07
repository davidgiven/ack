.define .rmu8
.sect .text
.sect .rom
.sect .data
.sect .bss

yh=8
xh=16
	! .rmu8 yields d0:d1 = remainder from x / y

.sect .text
.rmu8:
	move.l	d3, -(sp)	! preserve caller's d3
	move.l	(xh, sp), d0
	move.l	(yh, sp), d1
	sub.l	#4, sp
	jsr	(.divrem8)
	add.l	#4, sp
	move.l	d3, d1
	move.l	d2, d0
	move.l	(sp)+, d3
	rtd	#16
