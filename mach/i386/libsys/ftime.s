.sect .text; .sect .rom; .sect .data; .sect .bss
.define _ftime
.sect .text
_ftime:
	mov	ax,2856
	jmp	syscal
