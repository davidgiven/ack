.define	.fatal
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.fatal:
	jsr	(.trp)
	jmp	(__exit)
.align 2
