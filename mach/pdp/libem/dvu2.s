.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define dvu2~
! $Id$
dvu2~:
	clr	r0
	mov	04(sp),r1
	tst	02(sp)
	blt	1f
	div	02(sp),r0
2:	mov	(sp)+,r1
	add	$04,sp
	jmp	(r1)
1:	cmp	02(sp),r1
	bhi	2b
	inc	r0
	br	2b
