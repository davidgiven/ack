.define	.dup

	.text

!r2 will save return addres
.dup:
	movd tos, r2
	addr 0(sp), tos
	movd r0, tos
	jsr @.los
	movd r2, tos
	ret 0
