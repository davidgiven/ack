.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .sar2

.sar2:
	pop     bx              ! return address
				! si, descriptor address
				! ax, index
				! di, base address
	sub     ax,(si)
	mov     cx,4(si)
	imul    cx
	add     di,ax
	sar     cx,1
	jnb     1f
	pop     ax
	stosb
	jmp     bx
1:
	mov     si,sp
	rep movs
	mov     sp,si
	jmp     bx
