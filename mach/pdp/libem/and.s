/ $Header$
.text
.globl and~
.globl save1~

and~:
	mov	(sp)+,save1~
	mov	sp,r1
	add	r0,r1
	asr	r0
1:	com	(sp)
	bic	(sp)+,(r1)+
	sob	r0,1b
	jmp	*save1~
