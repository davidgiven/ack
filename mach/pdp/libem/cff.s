/ $Header$
.text
.globl cff~
.globl setfloat~

cff~:
	mov	(sp)+,r1
	mov	(sp)+,r0
	cmp	(sp)+,r0
	beq	1f
	jsr	pc,setfloat~
	movof	(sp)+,r0
	movf	r0,-(sp)
	setd
1:	jmp	(r1)
