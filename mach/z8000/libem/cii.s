.define cii
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

cii:
	popl	saveret, *RR14
	pop	R2, *RR14
	pop	R1, *RR14
	sub	R2, R1		!expansion in bytes
	jr	LE, 1f
	sra	R2		!expansion in words > 0
	jr	NC, 2f
	pop	R1, *RR14	!expand	1 --> 2
	extsb	R1
	push	*RR14, R1
	test	R2
	jr	EQ, 4f
2:				!expand	>= 1 word
	ld	R1, *RR14
	exts	RR0
3:	push	*RR14, R0
	djnz	R2, 3b
	jr	4f
1:	sub	R15, R2
4:	pushl	*RR14, saveret
	ret
