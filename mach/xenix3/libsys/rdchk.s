.sect .text; .sect .rom; .sect .data; .sect .bss
.define _rdchk
.sect .text
_rdchk:
	mov	ax,1832
	push	bp
	mov	bp,sp
	mov	bx,4(bp)
	call	syscal
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
1:
	pop	bp
	ret
