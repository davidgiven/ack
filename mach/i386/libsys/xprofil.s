.sect .text; .sect .rom; .sect .data; .sect .bss
.define __xprofil
.sect .text
__xprofil:
	mov	ax,44
	jmp	syscal
