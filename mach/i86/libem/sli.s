.define .sli

        ! $Header$
	! #bytes in ax
.sli:
	pop     dx              ! return address
	cmp     ax,2
	jne     1f
	pop     ax
	pop     cx
	sal     ax,cl
	push    ax
	jmp     dx
1:
	cmp     ax,4
	jne     9f
	pop     cx
	jcxz    2f
	pop     ax
	pop     bx
3:
	sal     ax,1
	rcl     bx,1
	loop    3b
	push    bx
	push    ax
2:
	jmp     dx
9:
	mov     ax,EODDZ
	push    ax
	jmp     .trp
