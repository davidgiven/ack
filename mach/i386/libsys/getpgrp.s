.sect .text; .sect .rom; .sect .data; .sect .bss
.define _getpgrp
.sect .text
_getpgrp:
	xor	ax,ax
	push	ax
	mov	ax,39
	call	syscal
	pop	bx
	ret
