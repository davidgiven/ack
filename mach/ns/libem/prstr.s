.define	.prstr

	.text

!het adres van de af te drukken string staat op de stack.
.prstr:
	save [r1]
	movd 8(sp), r1
1:
	cmpqb 0, 0(r1)
	beq 2f
	movb 0(r1), tos
	jsr @.putchar
	addqd 1, r1
	br 1b
2:
	restore [r1]
	ret 4
