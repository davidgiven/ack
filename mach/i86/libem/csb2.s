.define .csb2

.csb2:
        ! $Header$
				!si, descriptor address
				!dx,  index
	lods
	xchg    ax,bx           ! default
	lods
	xchg    ax,cx           ! number of cases
1:
	dec     cx
	jl      2f
	lods
	cmp     ax,dx
	lods
	jne     1b
	xchg    ax,bx
2:
	test    bx,bx
	jnz     3f
	mov     ax,ECASE
	push    ax
	jmp     .fat
3:
	jmp     bx
