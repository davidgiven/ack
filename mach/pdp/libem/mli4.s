.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define mli4~
.extern save~,retu~
! $Id$

mli4~:
	jsr	pc,save~
	mov	02(sp),r2
	sxt	r1
	sub	(sp),r1
	mov	06(sp),r0
	sxt	r3
	sub	04(sp),r3
	mul	r0,r1
	mul	r2,r3
	add	r1,r3
	mul	r2,r0
	sub	r3,r0
	add	$010,sp
	jmp	retu~
