.sect .text; .sect .rom; .sect .data; .sect .bss
.define _execve
.sect .text
_execve:
	mov	ax,59
	callf	0x7:0x0
	jmp	cerror
