.define csb
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

!R1 contains address of jump table
!R2 contains case index
csb:
	ld	R3, 0(R1)	!default pointer
	ld	R0, 2(R1)	!number of entries
	test	R0
	jr	EQ, 1f
3:	inc	R1, $4
	cp	R2, 0(R1)
	jr	EQ, 2f
	djnz	R0, 3b
1:	ld	R1, R3		!default pointer
	jr	4f
2:	ld	R1, 2(R1)
4:	test	R1
	jr	EQ, 5f
	jp	0(R1)
5:	push	*RR14, $ECASE
	jr	fatal
