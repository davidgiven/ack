.sect .text; .sect .rom; .sect .data; .sect .bss
.define _execve
.sect .text
_execve:
	mov	ax,59
	push	bp
	mov	bp,sp
	push	si
	mov	si,8(bp)
	mov	cx,6(bp)
	mov	bx,4(bp)
	call	syscal
	pop	si
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
1:
	pop	bp
	ret
