.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .isar

.isar:
	pop     bx
	pop     dx
	cmp     dx,2
.extern .unknown
	jne     .unknown
	pop     si      ! descriptor address
	pop     ax      ! index
	pop     di      ! array base
	push    bx
.extern .sar2
	jmp    .sar2
