.define xset

!bitnr in R1
!size (bytes) in R0
xset:
	popl	saveret, *RR14
	sra	R0
	ld	R2, R0
1:	push	*RR14, $0
	djnz	R0, 1b
	div	RR0, $020		!R0: bitnr, R1: wordnr
	cp	R1, R2
	jr	UGE, 2f
	ldk	R2, $0
	set	R2, R0
	sla	R1
	ld	RR14(R1), R2
3:	pushl	*RR14, saveret
	ret
2:	push	*RR14, $ESET
	calr	trp
	jr	3b
