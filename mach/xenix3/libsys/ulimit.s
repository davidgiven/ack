.sect .text; .sect .rom; .sect .data; .sect .bss
.define _ulimit
.sect .text
_ulimit:
	mov	ax,63
	push	bp
	mov	bp,sp
	push	si
	mov	si,8(bp)
	mov	cx,6(bp)
	mov	bx,4(bp)
	call	syscal
	pop	si
	pop	bp
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
	mov	dx,ax
	ret
1:
	mov	dx,bx
	ret
