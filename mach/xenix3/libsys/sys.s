.sect .text; .sect .rom; .sect .data; .sect .bss
.define syscal, sysal, sysn, sysnl, sysnx, sysc, syse, cerror, _errno

.sect .bss
	.comm	_errno, 2

.sect .text
syscal:
	int	5
	ret

sysal:
	push	bp
	mov	bp,sp
	push	di
	push	si
	mov	di,10(bp)
	mov	si,8(bp)
	mov	cx,6(bp)
	mov	bx,4(bp)
	call	syscal
	mov	dx,bx
	pop	si
	pop	di
	pop	bp
	jb	cerror
	ret

syse:
	mov	bx,sp
	add	bx,2
	call	syscal
cerror:
	mov	(_errno),ax
	mov	ax,-1
	cwd
	ret

sysn:
	mov	bx,sp
	add	bx,2
	call	syscal
	jb	cerror
	ret

sysnl:
	mov	bx,sp
	add	bx,2
	call	syscal
	jb	cerror
	mov	dx,bx
	ret

sysnx:
	mov	bx,sp
	add	bx,2
	call	syscal
	jb	cerror
	xchg	bx,ax
	ret

sysc:
	mov	bx,sp
	add	bx,2
	call	syscal
	jb	cerror
	xor	ax,ax
	cbw
	ret

