.sect .text; .sect .rom; .sect .data; .sect .bss
.define _locking
.sect .text
_locking:
	mov	ax,296
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	di,10(bp)
	mov	si,8(bp)
	mov	cx,6(bp)
	mov	bx,4(bp)
	call	syscal
	pop	di
	pop	si
	pop	bp
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
	mov	dx,ax
1:
	ret
