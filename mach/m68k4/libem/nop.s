.define .nop
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.nop:
	pea	4(sp)
	move.l	.lino,-(sp)
	pea	fmt
	jsr	.diagnos
	lea	12(sp),sp
	rts

	.sect .data
fmt:	.asciz "test %d, sp 0x%x.\n"
.align 2
