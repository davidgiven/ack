.sect .text; .sect .rom; .sect .data; .sect .bss
.define __exit
.sect .text
__exit:
	mov	ax,1
	push	bp
	mov	bp,sp
	mov	bx,4(bp)
	call	syscal
1:	jmp	1b
