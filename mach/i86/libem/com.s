.sect .text; .sect .rom; .sect .data; .sect .bss
.define	.com

	! #bytes in cx
	.sect .text
.com:
	mov	bx,sp
	add	bx,2
	sar	cx,1
1:
	not	(bx)
	inc	bx
	inc	bx
	loop	1b
	ret
