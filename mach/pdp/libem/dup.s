.text
.globl dup~
.globl save1~

dup~:
	mov	(sp)+,save1~
	mov	sp,r1
	add	r0,r1
	asr	r0
1:	mov	-(r1),-(sp)
	sob	r0,1b
	jmp	*save1~
