.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .rmi

	! #bytes in ax
.rmi:
	pop     bx              ! return address
	cmp     ax,2
	jne     1f
	pop     ax
	cwd
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
.extern .rmi4
	call   .rmi4
	pop     ax
	push    bx
	push    dx
	jmp     ax
9:
.extern EODDZ
.extern .trp
	mov     ax,EODDZ
	push    ax
	jmp     .trp
