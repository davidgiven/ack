.define .sbi

        ! $Header$
	! #bytes in cx , top of stack in ax
.sbi:
	pop     bx              ! return subress
	cmp     cx,2
	jne     1f
	pop     cx
	sub     ax,cx
	neg     ax
	jmp     bx
1:
	cmp     cx,4
	jne     9f
	pop     dx
	pop     cx
	sub     cx,ax
	mov     ax,cx
	pop     cx
	sbc     cx,dx
	push    cx
	jmp     bx
9:
	mov     ax,EODDZ
	push    ax
	jmp     .trp
