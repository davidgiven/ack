.sect .text; .sect .rom; .sect .data; .sect .bss
.define __stime
.sect .text
__stime:
	mov	ax,25
	push	bp
	mov	bp,sp
	mov	cx,6(bp)
	mov	bx,4(bp)
	call	syscal
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
1:
	pop	bp
	ret
