.sect .text; .sect .rom; .sect .data; .sect .bss
.define _sdenter
.sect .text
_sdenter:
	mov	ax,3880
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	cx,6(bp)
	mov	bx,4(bp)
	call	syscal
	pop	di
	pop	si
	pop	bp
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
1:
	ret
