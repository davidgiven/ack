.define .nop
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.nop:
	pea	4(sp)
	move.l	hol0,-(sp)
	pea	fmt
	jsr	.diagnos
	add.l	#12,sp
	rts

	.sect .data
fmt:	.asciz "test %d, sp %d\n"
.align 2
