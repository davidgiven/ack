.define _signal
.extern _signal
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
NSIG=32
_signal:
	move.l	4(sp), d0
	cmp.l	#NSIG,d0
	bcc	1f
	move.l	8(sp),d1
	move.l	d0,a0
	add.l	a0,a0
	add.l	a0,a0
	add.l	#dvect,a0
	move.l	(a0),a1
	move.l	d1,(a0)
	beq	2f
	btst	#0,d1
	bne	2f
	move.l	#jmptab,d1
	add.l	d0,d1
	add.l	d0,d1
2:
	move.l	d0,a0
	move.l	#0x30,d0
	trap	#0
	bcs	3f
	btst	#0,d0
	bne	4f
	move.l	a1,d0
4:
	clr.l	d1
	rts
1:
	move.l	#22,d0
3:
	jmp	cerror

jmptab:	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
	bsr	enter
enter:
	movem.l	d0/d1/a0/a1,-(sp)
	move.l	16(sp),d0
	sub.l	#jmptab+2,d0
	asr.l	#1,d0
	move.l	d0,-(sp)
	move.l	d0,a0
	add.l	a0,a0
	add.l	a0,a0
	add.l	#dvect,a0
	move.l	(a0),a0
	jsr	(a0)
	add.l	#4,sp
	movem.l	(sp)+,d0/d1/a0/a1
	add.l	#4,sp
	rtr
.sect .bss
dvect: .space 4*NSIG
