#
.define	.trp
.sect .text
.sect .rom
.sect .data
fmt:	.asciz "%s\n"
.sect .bss

	.sect .text
.trp:
	movem.l	d0/d1/a0, -(sp)
	move.l	(12, sp), a0	! return address
	move.l	(16, sp), d0	! error number
	move.l	a0, (16, sp)
	move.l	d0, (12, sp)
	cmp.l	#16, d0
	bcc	1f
	move.l	(.trpim), d1
	btst	d0, d1
	bne	3f
1:
	move.l	(.trppc), a0
	move.l	a0, d1
	beq	9f
	clr.l	(.trppc)
	movem.l d2-d7/a1-a5,-(sp)
#if _M68881
	fmovem.x fp0-fp7,-(sp)
#endif
	move.l	d0,-(sp)
	jsr	(a0)
	add.l	#4, sp
#if _M68881
	fmovem.x (sp)+,fp0-fp7
#endif
	movem.l (sp)+,d2-d7/a1-a5
3:
	movem.l	(sp)+, d0/d1/a0
	add.l	#4, sp
	rts
9:
	move.l	d0,-(sp)
	jsr	(__trpstr)
	move.l	d0,(sp)
	pea	(fmt)
	jsr	(.diagnos)
	lea	(4, sp), sp
	illegal
	jsr	(__exit)
.align 2
