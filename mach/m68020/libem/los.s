.define .los
.sect .text
.sect .rom
.sect .data
.sect .bss

	! d0 : # bytes
	! a0 : source address
	! a1 : return address
.sect .text
.los:
	move.l	(sp)+, a1
	move.l	(sp)+, d0
	move.l	(sp)+, a0
	cmp.l	#1, d0
	bne	1f
	clr.l	-(sp)
	move.b	(a0), (3,sp)	!1 byte
	bra	3f
1:
	cmp.l 	#2, d0
	bne	2f
	clr.l	-(sp)
	move.w	(a0), (2,sp)	!2 bytes
	bra	3f
2:
	add.l	d0, a0		!>=4 bytes
	asr.l 	#2, d0
	sub.l	#1, d0

4:	move.l	-(a0), -(sp)	
	dbf	d0, 4b
3:
	jmp	(a1)
.align 2
