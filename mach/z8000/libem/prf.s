.define prf
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

prf:
	ld	R0, hol0+4	!pointer to filename
	cp	R0, $0
	jr	EQ, 1f
	ld	R2, R0
	ld	R1, $40
3:	    !test filename on bad characters
	ldb	R3, 0(R2)
	cpb	R3, $0
	jr	EQ, 2f
	cpb	R3, $0177
	jr	GE, 1f
	cpb	R3, $040
	jr	LT, 1f
	inc	R2
	djnz	R1, 3b
	clrb	0(R2)
2:	push	*RR14, hol0
	ld	R1, R15
	push	*RR14, R1
	push	*RR14, R0
	push	*RR14, $fmt1
	calr	printf
	popl	saveprf, *RR14	!return address
	calr	printf		!because of call from 'noop'
	pushl	*RR14, saveprf
	ret
1:	ld	R0, $name
	jr	2b
.sect .data
fmt1:	.asciz	"%s, sp = %x, line %d:\n"
name:	.asciz	"_unknown file_"
saveprf:
	.data4 0
