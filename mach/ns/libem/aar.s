.define .aar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.aar:
	movd 8(sp), r7
	subd 0(4(sp)), r7
	muld 8(4(sp)), r7
	addd r7, 12(sp)
	ret 8
