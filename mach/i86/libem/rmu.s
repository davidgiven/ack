.define .rmu

        ! $Header$
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
	call   .rmu4
	pop     ax
	push    bx
	push    dx
	jmp     ax
9:
	mov     ax,EODDZ
	push    ax
	jmp     .trp
