.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .sti

	! #bytes in cx
	! destination address in di
.sti:
	pop     bx              ! return address
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
