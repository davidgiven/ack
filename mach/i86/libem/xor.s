.define	.xor

        ! $Header$
	! #bytes in cx
.xor:
	pop	bx		! return address
	mov	di,sp
	add	di,cx
	sar	cx,1
1:
	pop	ax
	xor	ax,(di)
	stos
	loop	1b
	jmp	bx
