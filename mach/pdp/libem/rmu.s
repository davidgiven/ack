.text
.globl rmu~
.globl save1~,rmu2~,rmu4~,unknown~

rmu~:
	mov	(sp)+,save1~
	cmp	r0,$04
	bgt	1f
	beq	2f
	cmp	r0,$02
	bne	1f
	jsr	pc,rmu2~
	mov	r1,-(sp)
	jmp	*save1~
2:	jsr	pc,rmu4~
	mov	r1,-(sp)
	mov	r0,-(sp)
	jmp	*save1~
1:	jmp	unknown~
