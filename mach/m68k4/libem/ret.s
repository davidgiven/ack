.define .ret

	.text
.ret:
	beq 3f
	cmp #2,d0
	bne 1f
	move (sp)+,d0
	bra 3f
1:
	cmp #4,d0
	bne 2f
	move.l (sp)+,d0
	bra 3f
2:
	cmp #8,d0
	bne 4f
	move.l (sp)+,d0
	move.l (sp)+,d1
3:
	unlk a6
	rts
4:
	move.w	#EILLINS,-(sp)
	jmp	.fat
.align 2
