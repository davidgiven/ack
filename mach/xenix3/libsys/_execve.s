.sect .text; .sect .rom; .sect .data; .sect .bss
.define __execve
.sect .text
__execve:
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
