.define .mon
.sect .text
.sect .rom
.sect .data
.sect .bss
	.sect .text
.mon:
	move.l	(sp)+,a0
	pea	fmt
	jsr	.diagnos
	add	#6,sp
	jmp	EXIT

	.sect .data
fmt:	.asciz "system call %d not implemented"
.align 2
