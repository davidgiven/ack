.define	.lar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.lar:
	movd tos, r2
	movd 4(sp), r0
	subd 0(0(sp)), r0
	muld 8(0(sp)), r0
	addd r0, 8(sp)
	movd 8(0(sp)), 4(sp)
	adjspd -4
	jsr @.los
	movd r2, tos
	ret 0
