.define los2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

los2:
	popl	saveret, *RR14
	pop	R0, *RR14		!object size
	ldk	R2, $0
	pop	R3, *RR14		!address of object
	cp	R0, $1
	jr	NE, 1f
	ldb	RL0, *RR2
	push	*RR14, R0
	jr	2f
1:	add	R3, R0
	dec	R3, $2
	dec	R15, $2
	sra	R0
	lddr	*RR14, *RR2, R0
	inc	R15, $2
2:	pushl	*RR14, saveret
	ret
