.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define mlu4~
.extern save~,retu~
! $Id$

mlu4~:
	jsr	pc,save~
	clr	r1
	mov	02(sp),r0
	mov	06(sp),r3
	mul	r3,r0
	tst	r3
	bge	1f
	add	02(sp),r0
1:	mov	02(sp),r3
	mul	04(sp),r3
	add	r3,r0
	mov	06(sp),r3
	mul	(sp),r3
	add	r3,r0
	add	$010,sp
	jmp	retu~
