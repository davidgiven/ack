.sect .text; .sect .rom; .sect .data; .sect .bss
.define .exg

	! #bytes in cx
.sect .text
.exg:
	push	di
	mov	sp,di
	add	di,4
	mov	bx,di
	add	bx,cx
	sar     cx,1
1:
	mov	ax,(bx)
	xchg	ax,(di)
	mov	(bx),ax
	loop	1b
2:
	pop	di
	ret
