.sect .text; .sect .rom; .sect .data; .sect .bss
.define __alarm
.sect .text
__alarm:
	mov	ax,27
	jmp	syscal
