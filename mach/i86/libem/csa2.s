.define .csa2

.csa2:
        ! $Header$
				! si, descriptor address
				! bx, index
	mov     dx,(si)         ! default
	sub     bx,2(si)
	cmp     bx,4(si)
	ja      1f
	sal     bx,1
	mov     bx,6(bx)(si)
	test    bx,bx
	jnz     2f
1:
	mov     bx,dx
	test    bx,bx
	jnz     2f
	mov     ax,ECASE
	push    ax
	jmp     .fat
2:
	jmp     bx
