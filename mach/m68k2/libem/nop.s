.define .nop
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.nop:
	move.w	hol0,-(sp)
	pea	fmt
	jsr	.diagnos
	add	#6,sp
	rts

	.sect .data
fmt:	.asciz "test %d\n"
.align 2
