.sect .text; .sect .rom; .sect .data; .sect .bss
.define cuf
.sect .rom
L1:	.data4 0x41f00000, 0x0
.sect .text
cuf:
	move.l	(sp)+,a0
	move.l	(sp)+,d1
	move.l	(sp)+,d0	! should be 4
	cmp.l	#4,d1
	bne	1f
	move.l	(sp),d0
	move.l	a0,-(sp)
	jsr	(Fflts)
	tst.l	d0
	bpl	2f
	move.l	#0x4f800000,d1
	jsr	(Fadds)
2:
	move.l	d0,(4,sp)
	rts
1:
	move.l	(sp),d0
	tst.l	-(sp)
	move.l	a0,-(sp)
	jsr	(Ffltd)
	tst.l	d0
	bpl	2f
	lea	(L1),a0
	jsr	(Faddd)
2:
	move.l	d1,(8,sp)
	move.l	d0,(4,sp)
	rts
