.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _abort
.extern _abort
_abort:	push	si
	push	di
	push	bp
	mov	bp,sp
	int	128
	mov	sp,bp
	pop	bp
	pop	di
	pop	si
	ret
