.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define csa~
.extern fat~
! $Id$

ECASE	= 024

csa~:
	sub	02(r0),r1
	blt	1f
	cmp	04(r0),r1
	blo	1f
	asl	r1
	add	r1,r0
	mov	06(r0),r1
	beq	2f
	jmp	(r1)
1:	mov	(r0),r0
	beq	2f
	jmp	(r0)
2:	mov	$ECASE,-(sp)
	jmp	fat~
