.sect .text; .sect .rom; .sect .data; .sect .bss
.define _ptrace
.sect .text
_ptrace:
	mov	ax,26
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	di,4(bp)
	mov	si,8(bp)
	mov	cx,6(bp)
	mov	bx,10(bp)
	mov	(_errno),0
	call	syscal
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
1:
	pop	di
	pop	si
	pop	bp
	ret
