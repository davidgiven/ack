.define	.prnl
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.prnl:	movb "\n", tos
	jsr @.putchar
	ret 0
