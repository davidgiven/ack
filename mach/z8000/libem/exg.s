.define exg
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

!size (bytes) in R0
exg:
	ldm	savereg, R4, $10
	ldl	RR2, RR14
	inc	R3, $2
	ldl	RR4, RR2
	add	R5, R0
	sra	R0
1:	ld	R1, *RR2
	ex	R1, *RR4
	ld	*RR4, R1
	inc	R3, $2
	inc	R5, $2
	djnz	R0, 1b
	ldm	R4, savereg, $10
	ret
