.define .lfr
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.lfr:
	move.l (sp)+,a0
	cmp #2,d7
	bne 1f
	move d0,-(sp)
	bra 3f
1:
	cmp #4,d7
	bne 2f
	move.l d0,-(sp)
	bra 3f
2:
	cmp #8,d7
	bne 4f
	move.l d1,-(sp)
	move.l d0,-(sp)
3:
	jmp(a0)
4:
	move.w	#EILLINS,-(sp)
	jmp	.fatal
.align 2
