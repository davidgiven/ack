.define dup
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

dup:
	popl	saveret, *RR14
	dec	R15, $2
	ldl	RR2, RR14
	add	R3, R0
	sra	R0
	lddr	*RR14, *RR2, R0
	inc	R15, $2
	pushl	*RR14, saveret
	ret
