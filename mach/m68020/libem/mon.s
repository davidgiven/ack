.define .mon
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.mon:
	move.l	(sp)+,a0
	pea	(fmt)
	jsr	(.diagnos)
	add.l	#8, sp
	jmp	(EXIT)

.sect .data
fmt:	.asciz "system call %d not implemented\n"
.align 2
