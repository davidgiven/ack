.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define	cmu4~
! $Id$
cmu4~:
	mov	(sp)+,r1
	clr	r0
	cmp	(sp),4(sp)
	bhi	1f
	blo	2f
	cmp	2(sp),6(sp)
	bhi	1f
	beq	3f
2:
	inc	r0
	br	3f
1:
	dec	r0
3:
	add	$010,sp
	jmp	(r1)
