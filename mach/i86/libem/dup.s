.sect .text; .sect .rom; .sect .data; .sect .bss
.define	.dup

	! #bytes in cx
	.sect .text
.dup:
	pop	bx		! return address
	mov	si,sp
	sub	sp,cx
	mov	di,sp
	sar	cx,1
	rep movs
	jmp	bx
