.sect .text; .sect .rom; .sect .data; .sect .bss
.define _lock
.sect .text
_lock:
	mov	ax,45
	jmp	sysal
