.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define csb~
.extern fat~
! $Id$

ECASE	= 024

csb~:
	mov	(r0)+,-(sp)
	mov	(r0)+,r3
	beq	1f
3:	cmp	(r0)+,r1
	beq	2f
	tst	(r0)+
	sob	r3,3b
1:	mov	(sp)+,r1
	br	4f
2:	tst	(sp)+
	mov	(r0),r1
4:	beq	5f
	jmp	(r1)
5:	mov	$ECASE,-(sp)
	jmp	fat~
