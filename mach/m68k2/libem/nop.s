.define .nop

	.text
.nop:
	move.w	hol0,-(sp)
	pea	fmt
	jsr	.diagnos
	add	#6,sp
	rts

	.data
fmt:	.asciz "test %d\n"
.align 2
