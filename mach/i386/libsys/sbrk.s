.sect .text; .sect .rom; .sect .data; .sect .bss
.define _sbrk
.sect .text
_sbrk:
	push	bp
	mov	bp,sp
	mov	ax,8(bp)
	or	ax,ax
	jne	1f
	mov	ax,(.limhp)
	pop	bp
	ret
1:
	mov	bx,(.limhp)
	add	ax,bx
	push	ax
	call	_brk
	or	ax,ax
	jne	1f
	pop	ax
	mov	(.limhp),ax
	mov	ax,bx
	pop	bp
	ret
1:
	pop	ax
	mov	ax,-1
	pop	bp
	ret
