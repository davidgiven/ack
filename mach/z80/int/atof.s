.define atof
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
	.sect .data
! Set of variables

big:	.data1 0
	.data1 0
	.data1 0x40
	.data1 24	! 2^23
negfrac:.space 1
negexp:	.space 1
begzero:
nd:	.space 2
fl:	.space 6
	exp=fl+4
eexp:	.space 2
flexp:	.space 4
exp5:	.space 4
endzero:
ten:	.data1 0
	.data1 0
	.data1 0x50
	.data1 4		! 10
dig:	.data1 0
	.data1 0
fildig:	.data1 0		! here a number from 0 to 31 will be converted flt.
	.data1 7
bexp:	.space 2

	.sect .text
atof:			! entry with stringpointer in hl
			! exit with pointer to float in hl
	push ix
	push iy
	push bc
	push de
	push af
	ld b,1
1:
	ld a,(hl)
	inc hl
	cp ' '
	jr z,1b
	cp '-'
	jr nz,1f
	ld b,-1
	jr 2f
1:	cp '+'
	jr z,2f
	dec hl
2:	ld a,b
	ld (negfrac),a
	xor a
	ld de,begzero
	ld b,endzero-begzero
1:	ld (de),a
	inc de
	djnz 1b
1:	ld a,(hl)
	inc hl
	sub '0'
	jr c,1f
	cp 10
	jr nc,1f
	ld (fildig),a
	call cmpbigfl
	jr z,2f
	call mulandadd
	jr 3f
2:	ld de,(exp)
	inc de
	ld (exp),de
3:	ld de,(nd)
	inc de
	ld (nd),de
	jr 1b
1:	cp '.'-'0'
	jr nz,4f
1:	ld a,(hl)
	inc hl
	sub '0'
	jr c,4f
	cp 10
	jr nc,4f
	ld (fildig),a
	call cmpbigfl
	jr z,2f
	call mulandadd
	ld de,(exp)
	dec de
	ld (exp),de
2:	ld de,(nd)
	inc de
	ld (nd),de
	jr 1b
4:
	ld b,1
	cp 'E'-'0'
	jr z,1f
	cp 'e'-'0'
	jr nz,5f
1:	ld a,(hl)
	inc hl
	cp '+'
	jr z,1f
	cp '-'
	jr nz,2f
	ld b,-1
	jr 1f
2:	dec hl
1:	ld a,b
	ld (negexp),a
	exx
	xor a
	ld h,a
	ld l,a
	ld b,a
	ld d,a
	ld e,a
	exx
1:	ld a,(hl)
	inc hl
	sub '0'
	jr c,1f
	cp 10
	jr nc,1f
	exx
	ld c,a
	add hl,hl
	add hl,hl
	add hl,de
	add hl,hl
	add hl,bc
	ld d,h
	ld e,l
	exx
	jr 1b
1:	exx
	ld hl,negexp
	or a
	bit 7,(hl)
	ld hl,(exp)
	jr z,1f
	sbc hl,de
	jr 2f
1:	add hl,de
2:	ld (exp),hl
	exx
5:	ld a,1
	ld de,(exp)
	push de
	bit 7,d
	jr z,1f
	neg
	ld hl,0
	or a
	sbc hl,de
	ex de,hl
1:	ld (negexp),a
	ld (exp),de
	pop de
	ld hl,(nd)
	add hl,de
	ld de,-33		! -LOGHUGE ?
	xor a
	sbc hl,de
	jp p,1f
	ld hl,fl
	ld b,6
2:	ld (hl),a
	inc hl
	djnz 2b
1:	ld hl,0x0140	! 1.0
	ld (flexp+2),hl
	ld hl,0x0350	! 5.0
	ld (exp5+2),hl
	ld hl,(exp)
	ld (bexp),hl
1:	bit 0,l
	jr z,2f
	call xflt
	.data2 flexp,exp5,fpmult,4,flexp
2:	sra h
	rr l
	ld a,h
	or l
	jr z,3f
	call xflt
	.data2 exp5,exp5,fpmult,4,exp5
	jr 1b
3:	ld hl,negexp
	ld a,(bexp)
	bit 7,(hl)
	jr z,1f
	call xflt
	.data2 flexp,fl,fpdiv,4,fl
	neg
	jr 2f
1:	call xflt
	.data2 flexp,fl,fpmult,4,fl
2:	ld b,a
	ld a,(fl+3)
	add a,b
	ld (fl+3),a
	ld a,(negfrac)
	bit 7,a
	jr z,1f
	call xflt
	.data2 fl,fl,fpcomp,4,fl
1:	call xflt
	.data2 fl,fl,fpnorm,4,fl
	ld hl,fl
	pop af
	pop de
	pop bc
	pop iy
	pop ix
	ret

cmpbigfl:
	call xflt
	.data2 big,fl,fpcmf,0
	ld a,(fpac+1)
	bit 7,a
	ret
mulandadd:
	call xflt
	.data2 fl,ten,fpmult,4,fl
	ld a,7
	ld (fildig+1),a
	call xflt
	.data2 dig,dig,fpnorm,4,dig
	call xflt
	.data2 fl,dig,fpadd,4,fl
	ret

xflt:
	ex (sp),iy
	push af
	push bc
	push de
	push hl
	ld h,(iy+1)
	ld l,(iy+0)
	ld de,fpac
	ld bc,4
	ldir
	ld h,(iy+3)
	ld l,(iy+2)
	ld de,fpop
	ld bc,4
	ldir
	push iy
	ld hl,1f
	push hl
	ld h,(iy+5)
	ld l,(iy+4)
	jp (hl)
1:	pop iy
	ld b,(iy+7)
	ld c,(iy+6)
	ld a,b
	or c
	jr z,1f
	inc iy
	inc iy
	ld hl,fpac
	ld d,(iy+7)
	ld e,(iy+6)
	ldir
1:	push iy
	pop hl
	ld de,8
	add hl,de
	push hl
	pop iy
	pop hl
	pop de
	pop bc
	pop af
	ex (sp),iy
	ret
.align 256
