.define	.com

        ! $Header$
	! #bytes in cx
.com:
	pop	bx		! return address
	mov	di,sp
	sar	cx,1
1:
	not	(di)
	inc	di
	inc	di
	loop	1b
	jmp	bx
