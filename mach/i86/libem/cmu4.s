.define .cmu4

.cmu4:
        ! $Header$
	pop     bx              ! return address
	xor     ax,ax
	pop     cx
	pop     dx
	pop     si
	pop     di
	cmp     di,dx
	ja      1f
	jb      2f
	cmp     si,cx
	ja      1f
	je      3f
2:
	dec     ax
3:
	jmp     bx
1:
	inc     ax
	jmp     bx

	ret
