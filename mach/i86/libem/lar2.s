.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .lar2

.lar2:
	pop     bx              ! return address
				! di, descriptor address
				! ax, index
				! si, base address
	sub     ax,(di)
	mov     cx,4(di)
	imul    cx
	add     si,ax
	sar     cx,1
	jnb     1f
	xorb    ah,ah
	lodsb
	push    ax
	jmp     bx
1:
	sub     sp,4(di)
	mov     di,sp
	rep movs
	jmp     bx
