.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define nop~
.extern hol0,prf~
! $Id$

nop~:
	mov	hol0,-(sp)
	mov	$fmt,-(sp)
	jsr	pc,prf~
	add	$04,sp
	rts	pc
.sect .data
fmt:	.asciz "test %d\n"
