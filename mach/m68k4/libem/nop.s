.define .nop

	.text
.nop:
	move.l	hol0,-(sp)
	pea	fmt
	jsr	.diagnos
	add.l	#8,sp
	rts

	.data
fmt:	.asciz "test %d\n"
.align 2
