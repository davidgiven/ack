.text
.globl los2~
.globl save1~

los2~:
	mov	(sp)+,save1~
	cmp	r0,$01
	bne	1f
	clr	-(sp)
	bisb	(r1),(sp)
	jmp	*save1~
1:	add	r0,r1
	asr	r0
2:	mov	-(r1),-(sp)
	sob	r0,2b
	jmp	*save1~
