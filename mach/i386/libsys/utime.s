.sect .text; .sect .rom; .sect .data; .sect .bss
.define _utime
.sect .text
_utime:
	mov	ax,30
	jmp	sysx
