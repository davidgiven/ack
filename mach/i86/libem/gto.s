.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .gto

.gto:
	mov     bp,4(bx)
	mov     sp,2(bx)
	jmp     (bx)
