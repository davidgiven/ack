.sect .text; .sect .rom; .sect .data; .sect .bss
.define .dvu

	! #bytes in ax
	.sect .text
.dvu:
	pop     bx              ! return address
	cmp     ax,2
	jne     1f
	pop     ax
	xor     dx,dx
	pop     cx
	div     cx
	push    ax
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
.extern .dvu4
	call   .dvu4
	pop     bx
	push    cx
	push    ax
	jmp     bx
9:
.extern EODDZ
.extern .trp
	mov     ax,EODDZ
	push    ax
	jmp     .trp
