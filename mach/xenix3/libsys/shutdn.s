.sect .text; .sect .rom; .sect .data; .sect .bss
.define _shutdn
.sect .text
_shutdn:
	mov	ax,40
	push	bp
	mov	bp,sp
	mov	cx,6(bp)
	mov	bx,4(bp)
	call	syscal
	mov	(_errno),ax
	mov	ax,-1
	pop	bp
	ret
