/ $Header$
.text
.globl inn~

inn~:
	mov	r0,-(sp)
	clr	r0
	div	$010,r0
	cmp	r0,(sp)
	bcc	1f
	add	sp,r0
	bitb	bits(r1),4(r0)
	beq	1f
	mov	$01,r0
	br	2f
1:	clr	r0
2:	mov	02(sp),r1
	add	(sp)+,sp
	tst	(sp)+
	jmp	(r1)
.data
bits:	.byte	1,2,4,10,20,40,100,200
