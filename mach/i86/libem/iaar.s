.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .iaar

.iaar:
	pop     cx
	pop     dx
	cmp     dx,2
.extern .unknown
	jne     .unknown
	pop     bx      ! descriptor address
	pop     ax      ! index
	sub     ax,(bx)
	mul     4(bx)
	pop	bx	! base address
	add     bx,ax
	push	cx
	ret
