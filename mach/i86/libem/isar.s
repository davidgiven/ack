.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .isar

.isar:
	pop     cx
	pop     ax
	cmp     ax,2
.extern .unknown
	jne     .unknown
	pop     bx      ! descriptor address
	pop     ax      ! index
	push    cx
.extern .sar2
	jmp    .sar2
