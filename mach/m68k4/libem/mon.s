.define .mon
	.text
.mon:
	move.l	(sp)+,a0
	pea	fmt
	jsr	.diagnos
	add	#6,sp
	jmp	EXIT

	.data
fmt:	.asciz "system call %d not implemented"
.align 2
