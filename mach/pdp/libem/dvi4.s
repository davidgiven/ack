.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define dvi4~
.extern save~,retu~
! $Id$

dvi4~:
	jsr	pc,save~
	mov	02(sp),r3
	sxt	r4
	bpl	1f
	neg	r3
1:	cmp	r4,(sp)
	bne	hardldiv
	mov	06(sp),r2
	mov	04(sp),r1
	bge	2f
	neg	r1
	neg	r2
	sbc	r1
	com	r4
2:	mov	r4,-(sp)
	clr	r0
	div	r3,r0
	mov	r0,-(sp)
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
	sxt	r1
	add	r1,r0
3:	mov	r0,r1
	mov	(sp)+,r0
	br	4f
hardldiv:
	clr	-(sp)
	mov	010(sp),r2
	mov	06(sp),r1
	bpl	5f
	com	(sp)
	neg	r1
	neg	r2
	sbc	r1
5:	clr	r0
	mov	02(sp),r3
	bge	6f
	neg	r3
	neg	04(sp)
	sbc	r3
	com	(sp)
6:	mov	$16,r4
9:	clc
	rol	r2
	rol	r1
	rol	r0
	cmp	r3,r0
	bhi	7f
	bcs	8f
	cmp	04(sp),r1
	blos	8f
7:	sob	r4,9b
	br	1f
8:	sub	04(sp),r1
	sbc	r0
	sub	r3,r0
	inc	r2
	sob	r4,9b
1:
	mov	r2,r1
	clr	r0
4:	tst	(sp)+
	beq	1f
	neg	r0
	neg	r1
	sbc	r0
1:	add	$010,sp
	jmp	retu~
