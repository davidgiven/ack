.text
.globl dvu~
.globl save1~,unknown~,dvu4~,dvu2~

dvu~:
	mov	(sp)+,save1~
	cmp	r0,$04
	bgt	1f
	beq	2f
	jsr	pc,dvu2~
	mov	r0,-(sp)
	br	3f
2:	jsr	pc,dvu4~
	mov	r1,-(sp)
	mov	r0,-(sp)
3:	jmp	*save1~
1:	jmp	unknown~
