/ $Header$
.text
.globl dup~

dup~:
	mov	(sp)+,r3
	mov	sp,r1
	add	r0,r1
	asr	r0
1:	mov	-(r1),-(sp)
	sob	r0,1b
	jmp	(r3)
