.sect .text; .sect .rom; .sect .data; .sect .bss
.define _alarm
.sect .text
_alarm:
	mov	ax,27
	jmp	syscal
