.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define rmi4~
.extern save~,retu~
! $Id$

rmi4~:
	jsr	pc,save~
	mov	02(sp),r3
	sxt	r4
	bpl	1f
	neg	r3
1:	cmp	r4,(sp)
	bne	hardrmi4
	mov	06(sp),r2
	mov	04(sp),r1
	mov	r1,r4
	bge	2f
	neg	r1
	neg	r2
	sbc	r1
2:	mov	r4,-(sp)
	clr	r0
	div	r3,r0
	mov	r1,r0
	mov	r1,r4
	mov	r2,r1
	div	r3,r0
	bvc	3f
	mov	r2,r1
	mov	r4,r0
	sub	r3,r0
	div	r3,r0
	tst	r1
	beq	3f
	add	r3,r1
3:	tst	(sp)+
	bpl	4f
	neg	r1
4:	sxt	r0
	br	9f
hardrmi4:
	mov	06(sp),r2
	mov	04(sp),r1
	bpl	5f
	neg	r1
	neg	r2
	sbc	r1
5:	clr	r0
	mov	(sp),r3
	bge	6f
	neg	r3
	neg	02(sp)
	sbc	r3
6:	mov	$16,r4
1:	clc
	rol	r2
	rol	r1
	rol	r0
	cmp	r3,r0
	bhi	7f
	bcs	8f
	cmp	02(sp),r1
	blos	8f
7:	sob	r4,1b
	br	2f
8:	sub	02(sp),r1
	sbc	r0
	sub	r3,r0
	sob	r4,1b
2:	tst	04(sp)
	bge	9f
	neg	r0
	neg	r1
	sbc	r0
9:	add	$010,sp
	jmp	retu~
