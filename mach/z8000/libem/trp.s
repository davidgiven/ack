.define trp, fatal

fatal:
	calr	trp
	sc	$EXIT

trp:
	push	*RR14, R1
	inc	R15, $2
	popl	saveret, *RR14
	pop	R1, *RR14	!trap number in R1
	pushl	*RR14, saveret
	push	*RR14, R0
	dec	R15, $2
	cp	R1, $16
	jr	UGE, 1f
	ld	R0, trpim
	bit	R0, R1
	jr	NE, 2f		!ignore
1:	sub	R15, $24
	ldm	*RR14, R2, $12
	push	*RR14, R1
	ld	R1, trppc
	cp	R1, $0
	jr	EQ, 3f
	clr	trppc
	call	0(R1)
	inc	R15, $2
	ldm	R2, *RR14, $12
	add	R15, $24
2:	pop	R1, *RR14
	pop	R0, *RR14
	ret
3:	push	*RR14, $err
	calr	printf
	sc	$EXIT
.data
err:	.asciz	"trap error %d\n"
