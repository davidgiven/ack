.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .iaar

.iaar:
	pop     bx
	pop     dx
	cmp     dx,2
.extern .unknown
	jne     .unknown
	pop     si      ! descriptor address
	pop     ax      ! index
	pop     di      ! array base
	sub     ax,(si)
	mul     4(si)
	add     di,ax
	jmp     bx
