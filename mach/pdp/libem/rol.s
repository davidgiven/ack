/ $Header$
.text
.globl rol~
.globl save~,retu~

rol~:
	jsr	pc,save~
	mov	(sp)+,r3
3:	add	r0,sp
	mov	r0,r1
	asr	r1
	clc
1:	rol	-(sp)
	sob	r1,1b
	bcc	2f
	mov	sp,r1
	add	r0,r1
	bis	$01,-(r1)
2:	sob	r3,3b
	jmp	retu~
