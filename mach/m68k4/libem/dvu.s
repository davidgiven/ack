.define .dvu
.sect .text
.sect .rom
.sect .data
.sect .bss

 ! unsigned long divide
 ! register usage:
 ! 	 : d2 divisor
 !         d1 dividend
 ! exit  : d1 quotient
 !         d0 remainder
	.sect .text
.dvu:
	move.l	4(sp),d2
	move.l	8(sp),d1
	move.l	d3,-(sp)
	tst.l	d2
	bne	0f
	move.l	#EIDIVZ,-(sp)
	jsr	.trp
0:
	clr.l	d0
	move.l	#31,d3
3:
	lsl.l	#1,d1
	roxl.l	#1,d0
	cmp.l	d2,d0
	blt	4f
	sub.l	d2,d0
	add	#1,d1
4:
	dbf	d3,3b
	move.l	(sp)+,d3
	move.l	(sp)+,a0
	add.l	#8,sp
	move.l	a0,-(sp)
	rts

.align 2
