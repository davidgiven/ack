.define rck
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

rck:
	ld	R0, RR14($4)
	cp	R0, 0(R1)
	jr	LT, 1f
	cp	R0, 2(R1)
	jr	LE, 2f
1:	push	*RR14, $ERANGE
	calr	trp
2:	ret
