.sect .text; .sect .rom; .sect .data; .sect .bss
.define __xstime
.sect .text
__xstime:
	mov	ax,25
	jmp	sysx
