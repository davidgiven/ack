.define .dvu
.sect .text
.sect .rom
.sect .data
.sect .bss

 ! unsigned long divide
 ! register usage:
 ! 	 : d0 divisor
 !         d1 dividend
 ! exit  : d1 quotient
 !         d2 remainder
	.sect .text
.dvu:
	move.l	4(sp),d0
	move.l	8(sp),d1
	move.l	d3,-(sp)
	tst.l	d0
	bne	0f
	move.w	#EIDIVZ,-(sp)
	jsr	.trp
0:
	clr.l	d2
	move.l	#31,d3
3:
	lsl.l	#1,d1
	roxl.l	#1,d2
	cmp.l	d0,d2
	blt	4f
	sub.l	d0,d2
	add	#1,d1
4:
	dbf	d3,3b
	move.l	(sp)+,d3
	move.l	(sp)+,a0
	add.l	#8,sp
	move.l	a0,-(sp)
	rts

.align 2
