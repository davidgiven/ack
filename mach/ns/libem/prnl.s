.define	.prnl

	.text

.prnl:	movb "\n", tos
	jsr @.putchar
	ret 0
