.sect .text; .sect .rom; .sect .data; .sect .bss
.define _geteuid
.sect .text
_geteuid:
	mov	ax,24
	call	syscal
	xchg	bx,ax
	ret
