.define	.fatal
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.fatal:
	jsr	(.trp)
	jmp	(_exit)
.align 2
