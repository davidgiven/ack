.sect .text; .sect .rom; .sect .data; .sect .bss
.define _times
.sect .text
_times:
	mov	ax,43
	push	bp
	mov	bp,sp
	mov	bx,4(bp)
	call	syscal
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
	mov	dx,ax
	pop	bp
	ret
1:
	mov	dx,bx
	pop	bp
	ret
