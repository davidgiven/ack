.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _execle
.extern _execle, _execve
_execle:
	push	si
	push	di
	push	bp
	mov	bp,sp
	lea	si,10(bp)
1:	mov	di,si
	add	si,2
	cmp	(di),0
	jne	1b
	push	(si)
	lea	ax,10(bp)
	push	ax
	push	8(bp)
	call	_execve
	add	sp,6
	mov	sp,bp
	pop	bp
	pop	di
	pop	si
	ret
