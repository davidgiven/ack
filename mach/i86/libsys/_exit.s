.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __exit
.sect .text
__exit:
	mov bx,sp
	xor ax,ax
	push ax	! unused memory
	push 2(bx)
	push ax	! dummy return address
	int 0x81
