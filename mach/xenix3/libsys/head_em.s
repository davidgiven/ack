.sect .text; .sect .rom; .sect .data; .sect .bss

.define begtext,begdata,begbss,syscal
.define hol0,.reghp,.limhp,.trppc,.ignmask
.define ERANGE,ESET,EHEAP,ECASE,EILLINS,EIDIVZ,EODDZ
.define EXIT,BRK
.extern endbss

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
syscal: jmp 1f
grow:	jmp 2f
	jmp 2f
	.data2 -277, -277, -277, -277, -277, -277
2:
	mov ax,2088
1:
	int 5
	jmp 1f
	.space 100
1:
	ret

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
	mov bx,endbss
	mov cx,9
	add bx,1024
	shr bx,cl
	shl bx,cl
	call grow
	xor	cx,cx
	push	cx
	call	__sbrk
	mov	(.limhp),ax
	mov	(.reghp),ax	! on Xenix, heap begins above stack!
	pop	ax
	call    __m_a_i_n
EXIT:
	push	ax
	call	__exit

BRK:
	jmp	__brk

.sect	.data
begdata:
hol0:
	.data2   0,0
	.data2   0,0
argv:
	.data2   3f
envp:
	.data2	0
3:
	.asciz "PROGRAM"
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
