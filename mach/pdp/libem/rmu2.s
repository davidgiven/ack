.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define rmu2~
! $Id$

rmu2~:
	mov	04(sp),r1
	tst	02(sp)
	blt	1f
	clr	r0
	div	02(sp),r0
2:	mov	(sp)+,r0
	add	$04,sp
	jmp	(r0)
1:	cmp	02(sp),r1
	bhi	2b
	sub	02(sp),r1
	br	2b
