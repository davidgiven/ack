.sect .text; .sect .rom; .sect .data; .sect .bss
.define	.com

	! #bytes in cx
	.sect .text
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
