.sect .text; .sect .rom; .sect .data; .sect .bss
.define	.dup

	! #bytes in cx
	.sect .text
.dup:
	pop	bx		! return address
	mov	ax,si
	mov	dx,di
	mov	si,sp
	sub	sp,cx
	mov	di,sp
	sar	cx,1
	rep movs
	mov	si,ax
	mov	di,dx
	jmp	bx
