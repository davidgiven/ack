.define .sdret, .dsret, .sret, .dret, .cret 
.sect .text

.dsret:
	pop	edi
.sret:
	pop	esi
.cret:
	mov	esp,ebp
	pop	ebp
	ret

.sdret:
	pop	esi
.dret:
	pop	edi
	jmp	.cret
