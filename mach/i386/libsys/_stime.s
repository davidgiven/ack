.sect .text; .sect .rom; .sect .data; .sect .bss
.define __stime
.sect .text
__stime:
	mov	ax,25
	jmp	sysx
