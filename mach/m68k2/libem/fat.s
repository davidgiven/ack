.define	.fat
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.fat:
	jsr	.trp	
	jmp	EXIT
