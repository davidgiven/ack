.sect .text; .sect .rom; .sect .data; .sect .bss
.define __execve
.sect .text
__execve:
	mov	ax,59
	callf	0x7:0x0
	jmp	cerror
