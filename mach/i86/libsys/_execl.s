.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text

.define __execl
.extern __execl, _environ, __execve
__execl:
	push	si
	push	di
	push	bp
	mov	bp,sp
	push	(_environ)
	lea	ax,10(bp)
	push	ax
	push	8(bp)
	call	__execve
	add	sp,6
	mov	sp,bp
	pop	bp
	pop	di
	pop	si
	ret
