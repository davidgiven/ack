.sect .text; .sect .rom; .sect .data; .sect .bss
.define _nap
.sect .text
_nap:
	mov	ax,3112
	push	bp
	mov	bp,sp
	mov	cx,6(bp)
	mov	bx,4(bp)
	call	syscal
	pop	bp
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
	mov	dx,ax
	ret
1:
	mov	dx,bx
	ret
