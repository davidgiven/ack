.sect .text; .sect .rom; .sect .data; .sect .bss
.define _sbrk
.sect .text
_sbrk:
	push	bp
	mov	bp,sp
	push	ds
	mov	ax,4(bp)
	cwd
	push	dx
	push	ax
	mov	ax,1
	push	ax
	call	__brkctl
	add	sp,8
	pop	bp
	ret


__brkctl:
	mov	ax,4904
	jmp	sysal
