.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .rmu

	! #bytes in ax
.rmu:
	pop     bx              ! return address
	cmp     ax,2
	jne     1f
	pop     ax
	xor     dx,dx
	pop     cx
	idiv    cx
	push    dx
	jmp     bx
1:
	cmp     ax,4
	jne     9f
	pop     ax
	pop     dx
	pop     si
	pop     di
	push    bx
	push    di
	push    si
	push    dx
	push    ax
.extern .rmu4
	call   .rmu4
	pop     bx
	push    dx
	push    ax
	jmp     bx
9:
.extern EODDZ
.extern .trp
	mov     ax,EODDZ
	push    bx
	jmp     .trp
