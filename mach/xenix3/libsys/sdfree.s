.sect .text; .sect .rom; .sect .data; .sect .bss
.define _sdfree
.sect .text
_sdfree:
	mov	ax,3624
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	bx,4(bp)
	call	syscal
	pop	di
	pop	si
	pop	bp
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
1:
	ret
