.sect .text; .sect .rom; .sect .data; .sect .bss
.define __brk
.sect .text
__brk:
	mov	ax,17
	push	bp
	mov	bp,sp
	mov	bx,4(bp)
	call	syscal
	mov	dx,bx
	pop	bp
	jb	cerror
	mov	bx,sp
	mov	dx,2(bx)
	mov	(.limhp),dx
	ret
