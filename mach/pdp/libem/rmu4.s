.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define rmu4~
.extern save~,retu~
! $Id$

rmu4~:
	jsr	pc,save~
	clr	r0
	tst	(sp)
	bne	hardrmu4
	tst	02(sp)
	blt	hardrmu4
	mov	06(sp),r2
	mov	04(sp),r1
	mov	02(sp),r3
	div	r3,r0
	mov	r1,r0
	mov	r1,r4
	mov	r2,r1
	div	r3,r0
	bvc	1f
	mov	r2,r1
	mov	r4,r0
	sub	r3,r0
	div	r3,r0
	tst	r1
	beq	1f
	add	r3,r1
1:	clr	r0
	br	2f
hardrmu4:
	mov	06(sp),r2
	mov	04(sp),r1
	mov	(sp),r3
	mov	$17,r4
	br	3f
6:	clc
	rol	r2
	rol	r1
	rol	r0
3:	cmp	r3,r0
	bhi	4f
	bcs	5f
	cmp	02(sp),r1
	blos	5f
4:	sob	r4,6b
	br	2f
5:	sub	02(sp),r1
	sbc	r0
	sub	r3,r0
	sob	r4,6b
2:	add	$010,sp
	jmp	retu~
