.sect .text; .sect .rom; .sect .data; .sect .bss
.define _sync
.sect .text
_sync:
	mov	ax,36
	call	syscal
	ret
