.define Lxl,Lxa2
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! AccB: number of static links to follow.
! AccAB: local base (Lxl) or argument base (Lxa2).

Lxl:
	bsr Walk
	jmp TfrXD

Lxa2:
	bsr Walk
	inx
	inx
	inx
	inx
	jmp TfrXD

Walk:
	ldx <LB
	tstb
	beq 2f
1:
	ldx EM_BSIZE,x
	decb
	bne 1b
2:
	rts
