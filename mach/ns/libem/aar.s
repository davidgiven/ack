.define .aar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.aar:
	movd 8(sp), r3
	subd 0(4(sp)), r3
	muld 8(4(sp)), r3
	addd r3, 12(sp)
	ret 8
