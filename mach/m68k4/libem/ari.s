.define .sari
.define .lari
.define .aari

	.text
.aari:
	move.l	(sp)+,.retari	! return address
	cmp.l	#4, (sp)+	! size of descriptor elements
	bne	9f
	jsr	.aar
	move.l	.retari,-(sp)
	rts


.lari:
	move.l	(sp)+,.retari	! return address
	cmp.l	#4, (sp)+	! size of descriptor elements
	bne	9f
	jsr	.lar
	move.l	.retari,-(sp)
	rts
9:
	move.w	#EILLINS,-(sp)
	jmp	.fat


.sari:
	move.l	(sp)+,.retari	! return address
	cmp.l	#4, (sp)+	! size of descriptor elements
	bne	9b
	jsr	.sar
	move.l	.retari,-(sp)
	rts
.data
.retari:	.long 0
.align 2
