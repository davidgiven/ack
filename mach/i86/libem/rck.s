.define .rck

        ! $Header$
	! descriptor address in si
	! value in ax, must be left there
.rck:
	cmp     ax,(si)
	jl      2f
	cmp     ax,2(si)
	jg      2f
	ret
2:
	push    ax
	mov     ax,ERANGE
	call    .error
	pop     ax
	ret
