.define .sari
.define .lari
.define .aari
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.aari:
	move.l	(sp)+,d0	! return address
	cmp.l	#4, (sp)+	! size of descriptor elements
	bne	9f
	move.l	d0,-(sp)
	jmp	.aar


.lari:
	move.l	(sp)+,d0	! return address
	cmp.l	#4, (sp)+	! size of descriptor elements
	bne	9f
	move.l	d0,-(sp)
	jmp	.lar
9:
	move.w	#EILLINS,-(sp)
	jmp	.fat


.sari:
	move.l	(sp)+,d0	! return address
	cmp.l	#4, (sp)+	! size of descriptor elements
	bne	9b
	move.l	d0,-(sp)
	jmp	.sar

.align 2
