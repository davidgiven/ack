.sect .text; .sect .rom; .sect .data; .sect .bss

.define begtext,begdata,begbss
.define hol0,.reghp,.limhp,.trppc,.ignmask
.define ERANGE,ESET,EHEAP,ECASE,EILLINS,EIDIVZ,EODDZ
.define EXIT, BRK

ERANGE          = 1
ESET            = 2
EIDIVZ          = 6
EHEAP           = 17
EILLINS         = 18
EODDZ           = 19
ECASE           = 20

.sect .text
	.data1	0353
	.data1	0176	! jmp to begtext in 2 bytes. Assembler generates 3
			! bytes.
	.data2 -277, -277, -277, -277
	.space 116
	.data2 49920

begtext:
	mov	(.limhp),ax
	mov	(.reghp),ax
	xor	bp,bp
	mov bx,sp
	mov cx,(bx)
	add bx,4
	mov ax,cx
	inc ax
	shl ax,2
	add ax,bx
	push ax
	push bx
	push cx
	call    __m_a_i_n
	push	ax
EXIT:
	call	__exit

BRK:
	jmp	__brk

.sect	.data
begdata:
hol0:
	.data2   0,0
	.data2   0,0
argv:
	.data4   3f
envp:
	.data4	0
3:
	.asciz "PROGRAM"
.reghp:
	.data4   endbss
.limhp:
	.data4   endbss
.ignmask:
	.data4   0
.trppc:
	.data4   0

	.sect .bss
begbss:
