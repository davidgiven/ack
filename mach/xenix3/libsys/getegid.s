.sect .text; .sect .rom; .sect .data; .sect .bss
.define _getegid
.sect .text
_getegid:
	mov	ax,47
	call	syscal
	xchg	bx,ax
	ret
