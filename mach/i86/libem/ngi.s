.define .ngi

        ! $Header$
	! #bytes in ax
.ngi:
	pop     bx              ! return address
	cmp     ax,2
	jne     1f
	pop     cx
	neg     cx
	push    cx
	jmp     bx
1:
	cmp     ax,4
	jne     9f
	pop     dx
	pop     ax
	neg     ax
	neg     dx
	sbb     ax,0
	push    dx
	push    ax
	jmp     bx
9:
	mov     ax,EODDZ
	push    ax
	jmp     .trp
