.define .nop

	.text
.nop:
	pea	4(sp)
	move.l	hol0,-(sp)
	pea	fmt
	jsr	.diagnos
	add.l	#12,sp
	rts

	.data
fmt:	.asciz "test %d, sp %d\n"
.align 2
