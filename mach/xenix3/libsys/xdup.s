.sect .text; .sect .rom; .sect .data; .sect .bss
.define ___xdup
.sect .text
___xdup:
	push	bp
	mov	bp,sp
	mov	ax,41
	mov	cx,6(bp)
	mov	bx,4(bp)
	call	syscal
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
1:
	pop	bp
	ret
