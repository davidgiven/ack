.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _signal
.extern _signal, cerror
NSIG=16
_signal:	mov	bx,sp
	mov cx,4(bx)
	mov bx,2(bx)
	cmp bx,NSIG
	jae 1f
	shl bx,1
	mov dx,dvect(bx)
	cmp cx,1
	jbe 2f
	mov dvect(bx),cx
	mov bx,sp
	mov 4(bx),entry
	mov bx,dx
	int 0xb0
	mov dx,bx
	mov bx,sp
	mov 4(bx),cx
	jb 3f
	jmp 4f
2:
	int 0xb0
	jb 3f
	mov dvect(bx),cx
4:
	cmp ax,1
	jbe 5f
	mov ax,dx
5:
	ret
1:
	mov ax,22
3:
	jmp cerror

entry:
	push bx
	push cx
	push dx
	push di
	push si
	mov bx,sp
	mov di,10(bx)
	mov 10(bx),ax
	push di
	shl di,1
	call dvect(di)
	add sp,2
	pop si
	pop di
	pop dx
	pop cx
	pop bx
	pop ax
	popf
	ret
.sect .bss
dvect:	.space 2*NSIG
.sect .text
