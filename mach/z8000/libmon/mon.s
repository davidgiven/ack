.define mon
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

mon:
	popl	saveret, *RR14
	pop	R0, *RR14	! iocode
	cp	R0, $1		! exit
	jr	NE, read
	inc	R15, $2
	sc	$0
read:	cp	R0, $3		! read
	jr	NE, write
	pop	R0, *RR14	! dummy;  all input from stdin
	pop	R1, *RR14	! ptr to buffer
	pop	R2, *RR14	! nr of bytes to be read
	ld	R3, R1
	cp	R2, $0
	jr	EQ, 6f
1:	sc	$2		! read character into RL0
	cpb	RL0, $004	! \^D
	jr	EQ, 6f
	cpb	RL0, $015	! \cr
	jr	NE, 2f
	ldb	RL0, $012
2:	sc	$4		! echo
	cpb	RL0, $010	! \^H
	jr	NE, 3f
	cp	R1, R3
	jr	EQ, 5f
	dec	R1
	jr	5f
3:	cpb	RL0, $0100	! \@
	jr	NE, 4f
	ld	R1, R3
	ldb	RL0, $012
	sc	$4
	jr	5f
4:	ldb	0(R1), RL0
	inc	R1
	cpb	RL0, $012	! \nl
	jr	EQ, 6f
5:	djnz	R2, 1b
6:	sub	R1, R3		! nr of bytes read
	push	*RR14, R1
	push	*RR14, $0
	jr	retu
write:	cp	R0, $4		! write
	jr	NE, open
	pop	R0, *RR14	! dummy;  all output to stdout
	pop	R1, *RR14	! ptr to buffer
	pop	R2, *RR14	! nr of bytes to be written
	ld	R3, R2
	cp	R2, $0
	jr	EQ, 8f
9:	ld	R0, $5000	! counter to delay printing a little
7:	djnz	R0, 7b
	ldb	RL0, 0(R1)
	sc	$4
	inc	R1
	djnz	R2, 9b
8:	sub	R3, R2		! nr of bytes written
	push	*RR14, R3
	push	*RR14, $0
	jr	retu
open:	cp	R0, $5		! open
	jr	close
	jr	NE, close
	ld	*RR14, $0
	ld	2(R15), $0
	jr	retu
close:	cp	R0, $6		! close
	jr	NE, ioctl
	ld	*RR14, $0
	jr	retu
ioctl:	cp	R0, $54		! ioctl
	jr	NE, err
	inc	R15, $4
	ld	*RR14, $0
retu:	ldl	RR2, saveret
	jp	*RR2
err:	push	*RR14, saveret
	push	*RR14, $EBADMON
	calr	trp
	ret
