.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define cif
cif:
	move.l	(sp)+,a0
	move.l	(sp)+,d1
	move.l	(sp)+,d0	! should be 4
	cmp.l	#4,d1
	bne	1f
	move.l	(sp),d0
	move.l	a0,-(sp)
	jsr	(Fflts)
	move.l	d0,(4,sp)
	rts
1:
	move.l	(sp),d0
	tst.l	-(sp)
	move.l	a0,-(sp)
	jsr	(Ffltd)
	move.l	d1,(8,sp)
	move.l	d0,(4,sp)
	rts

