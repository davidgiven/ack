.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define cuu~
! $Id$

cuu~:
	mov	(sp)+,r1
	mov	(sp)+,r0
	sub	(sp)+,r0
	ble	1f
	asr	r0
2:	clr	-(sp)
	sob	r0,2b
1:	sub	r0,sp	! if out of sob loop r0==0
	jmp	(r1)
