.sect .text; .sect .rom; .sect .data; .sect .bss
.define	.and

	! #bytes in cx
	.sect .text
.and:
	pop	bx		! return address
	mov	di,sp
	add	di,cx
	sar	cx,1
1:
	pop	ax
	and	ax,(di)
	stos
	loop	1b
	jmp	bx
