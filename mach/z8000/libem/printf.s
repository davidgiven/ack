.define printf
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

printf:
	popl	saveret, *RR14
	ldm	savereg, R4, $10
	ld	R3, $buff	!R3 is pointer to a buffer, in which 
				!we built the string to be printed.
	pop	R2, *RR14	!R2 is pointer to format-string
prloop:
	ldb	RL0, 0(R2)
	testb	RL0
	jr	EQ, ready
	inc	R2
	cpb	RL0, $045	! '%'?
	jr	NE, 1f
	ldb	RL0, 0(R2)
	inc	R2
	cpb	RL0, $0163	! 's'?
	jr	EQ, 3f
	cpb	RL0, $0170	! 'x'?
	ld	R4, $16		! print number hexadecimal
	jr	EQ, 2f
	cpb	RL0, $0144	! 'd'?
	ld	R4, $10		! print number decimal
	jr	EQ, 2f
	cpb	RL0, $0157	! 'o'?
	ld	R4, $8		! print number octal
	jr	EQ, 2f
1:	ldb	0(R3), RL0
	inc	R3
	jr	prloop
2:	    !in case of %x, %d or %o
	pop	R1, *RR14
	test	R1
	jr	PL, 4f
	cp	R4, $10
	jr	NE, 4f		! print only '-' in case of %d
	ldb	0(R3), $055	! '-'
	inc	R3
	neg	R1
4:	calr	printn
	jr	prloop
3:	    !in case of %s
	pop	R1, *RR14
6:	ldb	RL0, 0(R1)
	testb	RL0
	jr	EQ, prloop
	inc	R1
	ldb	0(R3), RL0
	inc	R3
	jr	6b
ready:	    !now really print the string we built in the buffer
	ldb	0(R3), RL0	!end string with '\0'
	sub	R3, $buff-1	!R3 contains the number of characters
	ld	R1, $buff

	push	*RR14, R3	!count
	push	*RR14, R1	!buffer
	push	*RR14, $2	!file descriptor
	push	*RR14, $4	! write
	calr	mon
	pop	R1, *RR14
	pop	R1, *RR14	! ignore result

	ldm	R4, savereg, $10
	pushl	*RR14, saveret
	ret

printn:
	ldk	R0, $0
	div	RR0, R4		!%x, %d or %o determined by R4
	test	R1
	jr	EQ, 5f		!if quotient is '0' printn is ready
	push	*RR14, R0	!push remainder onto the stack
	calr	printn
	pop	R0, *RR14
5:	add	R0, $060
	cp	R0, $071	!'9'
	jr	LE, 8f
	add	R0, $7
8:	ldb	0(R3), RL0
	inc	R3
	ret

.sect .data
buff:
	.space 256
