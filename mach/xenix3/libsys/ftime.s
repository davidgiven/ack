.sect .text; .sect .rom; .sect .data; .sect .bss
.define _ftime
.sect .text
_ftime:
	mov	ax,2856
	push	bp
	mov	bp,sp
	mov	bx,4(bp)
	call	syscal
	pop	bp
	ret

