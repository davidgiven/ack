.sect .text; .sect .rom; .sect .data; .sect .bss
.define __xtime
.sect .text
__xtime:
	mov	ax,13
	jmp	syscal
