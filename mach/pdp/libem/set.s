/ $Header$
.text
.globl set~
.globl save~,retu~,trp~

ESET	= 2

set~:
	jsr	pc,save~
	mov	r0,r2
	asr	r0
1:	clr	-(sp)
	sob	r0,1b
	div	$8.,r0
	cmp	r0,r2
	blo	2f
	mov	$ESET,-(sp)
	jsr	pc,trp~
	jmp	retu~
2:	add	sp,r0
	bisb	bits(r1),(r0)
	jmp	retu~

.data
bits:	 .byte	1,2,4,10,20,40,100,200
