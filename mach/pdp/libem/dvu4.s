.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define dvu4~
.extern save~,retu~
! $Id$

dvu4~:
	jsr	pc,save~
	clr	r0
	tst	(sp)
	bne	harddvu4
	tst	02(sp)
	blt	harddvu4
	mov	06(sp),r2
	mov	04(sp),r1
	mov	02(sp),r3
	div	r3,r0
	mov	r0,-(sp)
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
	sxt	r1
	add	r1,r0
1:	mov	r0,r1
	mov	(sp)+,r0
	br	2f
harddvu4:
	mov	06(sp),r2
	mov	04(sp),r1
	mov	(sp),r3
	mov	$17,r4
	br	3f
6:	rol	r2
	rol	r1
	rol	r0
3:	cmp	r3,r0
	bhi	4f
	blo	5f
	cmp	02(sp),r1
	blos	5f
4:	clc
	sob	r4,6b
	br	7f
5:	sub	02(sp),r1
	sbc	r0
	sub	r3,r0
	sec
	sob	r4,6b
7:	rol	r2
	bcc	8f
	mov	$01,r0
	br	9f
8:	clr	r0
9:	mov	r2,r1
2:	add	$010,sp
	jmp	retu~
