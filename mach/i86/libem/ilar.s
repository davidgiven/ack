.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .ilar

.ilar:
	pop     bx
	pop     dx
.extern .unknown
	cmp     dx,2
	jne     .unknown
	pop     di      ! descriptor address
	pop     ax      ! index
	pop     si      ! array base
	push    bx
.extern .lar2
	jmp    .lar2
