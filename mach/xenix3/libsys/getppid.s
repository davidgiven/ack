.sect .text; .sect .rom; .sect .data; .sect .bss
.define _getppid
.sect .text
_getppid:
	mov	ax,20
	call	syscal
	xchg	bx,ax
	ret
