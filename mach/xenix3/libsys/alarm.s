.sect .text; .sect .rom; .sect .data; .sect .bss
.define _alarm
.sect .text
_alarm:
	mov	ax,27
	push	bp
	mov	bp,sp
	mov	bx,4(bp)
	call	syscal
	pop	bp
	ret
