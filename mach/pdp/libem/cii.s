.text
.globl cii~
.globl save1~

/convert int to int
/ 1 byte -> ? : sign extension
cii~:
	mov	(sp)+,save1~
	mov	(sp)+,r0
	sub	(sp)+,r0
	ble	1f
	asr	r0
	bcc	2f
	movb	(sp),r1
	mov	r1,(sp)
2:	tst	r0
	beq	3f
	tst	(sp)
4:	sxt	-(sp)
	sob	r0,4b
3:	jmp	*save1~
1:	sub	r0,sp
	jmp	*save1~
