.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define rck~
.extern trp~
! $Id$

ERANGE	= 1

rck~:
	mov	(sp)+,r1
	cmp	(sp),(r0)
	blt	1f
	cmp	(sp),02(r0)
	ble	2f
1:	mov	$ERANGE,-(sp)
	jsr	pc,trp~
2:	jmp	(r1)
