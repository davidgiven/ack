.sect .text; .sect .rom; .sect .data; .sect .bss

.define begtext,begdata,begbss
.define hol0,.reghp,.limhp,.trppc,.ignmask
.define ERANGE,ESET,EHEAP,ECASE,EILLINS,EIDIVZ,EODDZ

ERANGE          = 1
ESET            = 2
EIDIVZ          = 6
EHEAP           = 17
EILLINS         = 18
EODDZ           = 19
ECASE           = 20

.sect .text
begtext:
	mov bx,sp
	mov cx,(bx)
	add bx,2
	mov ax,cx
	inc ax
	shl ax,1
	add ax,bx
	push ax
	push bx
	push cx
	xor bp,bp
	call    __m_a_i_n
	add sp,6
	call	__exit
.sect	.data
begdata:
hol0:
	.data2   0,0
	.data2   0,0
.reghp:
	.data2   endbss
.limhp:
	.data2   endbss
.ignmask:
	.data2   0
.trppc:
	.data2   0

	.sect .bss
begbss:
