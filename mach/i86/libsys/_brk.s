.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __brk
.define xbrk
.define np
.extern __brk
.extern xbrk,endbss
.extern np, cerror
__brk:	
	mov bx,sp
	mov ax,2(bx)
	mov cx,sp
	sub cx,128
	jbe 1f
	mov bx,(np)
	mov (np),ax
	sub ax,bx
	jbe 2f
	call xbrk
2:
	xor ax,ax
	ret
1:
	mov ax,0xc
	jmp cerror
xbrk:
	push di
	mov di,bx
	mov cx,ax
	xor ax,ax
	shr cx,1
	repz stos
	jae 3f
	stosb
3:
	pop di
	ret
.sect .data
np: .data2 endbss
.sect .text
