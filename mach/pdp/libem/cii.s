.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define cii~
! $Id$

!convert int to int
! 1 byte -> ? : sign extension
cii~:
	mov	(sp)+,r3
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
1:	sub	r0,sp	! if out of sob loop r0==0
3:	jmp	(r3)
