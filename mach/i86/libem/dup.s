.define	.dup

        ! $Header$
	! #bytes in cx
.dup:
	pop	bx		! return address
	mov	si,sp
	sub	sp,cx
	mov	di,sp
	sar	cx,1
	rep movs
	jmp	bx
