.sect .text; .sect .rom; .sect .data; .sect .bss
.define __ftime
.sect .text
__ftime:
	mov	ax,2856
	jmp	syscal
