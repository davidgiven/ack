.text
.globl sto2~
.globl save1~

sto2~:
	mov	(sp)+,save1~
	cmp	r0,$01
	bne	1f
	movb	(sp),(r1)
	tst	(sp)+
	jmp	*save1~
1:	asr	r0
2:	mov	(sp)+,(r1)+
	sob	r0,2b
	jmp	*save1~
