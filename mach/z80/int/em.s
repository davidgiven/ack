#
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
! This program is an EM interpreter for the Z80.
! Register pair bc is used to hold lb.
! Register ix is used to hold the EM program counter.
! The interpreter assumes 16-bit words and 16-bit pointers.

! #define	CPM1	1

! Definitions:
   zone = 8	! size of subroutine call block (address + old lb)
   bdos = 5	! standard entry into I/O-routines
   boot = 0
   fcb = 0x5c	! file descriptor of EM-1 file (5C hex)

   reset=0
   delete=19
   makefile=22
   close=16
   readconsole = 10
   writeconsole = 2
   open = 15
   read = 20
   write = 21
   setdma = 26
   printstring = 9
   seqread = 20
   randomread = 33
   seqwrite = 21
   randomwrite = 34
   consolein = 1
   diconio = 6
   RAW=0			!0 for cooked,1 for raw io

   timebuf=0xFFDE

   b_lolp = 176
   b_loln = 179
   b_lof = 161
   b_loi = 168
   b_lal = 130
   b_lil = 146
   b_stlm = 227
   b_stf = 214
   b_sti = 218
   b_inl = 112
   b_cal = 63
   b_asp = 44
   b_zrl = 249

   EARRAY = 0
   ERANGE = 1
   EILLINS=18
   EILLSIZE=19
   ECASE=20
   EBADMON=25

!--------------------------- Initialization ---------------------------

	jp init		! 3 byte instruction.

!------------------------- MAIN DISPATCH ------------------------------
!
! must be put in a suitable place in memory to reduce memory usage
! must be put on a page boundary


.data1 0		! fourth byte
dispat = . - 4	! base of dispatch table
!	.data2	loc.0
!	.data2	loc.1
	.data2	loc.2
	.data2	loc.3
	.data2	loc.4
	.data2	loc.5
	.data2	loc.6
	.data2	loc.7
	.data2	loc.8
	.data2	loc.9
	.data2	loc.10
	.data2	loc.11
	.data2	loc.12
	.data2	loc.13
	.data2	loc.14
	.data2	loc.15
	.data2	loc.16
	.data2	loc.17
	.data2	loc.18
	.data2	loc.19
	.data2	loc.20
	.data2	loc.21
	.data2	loc.22
	.data2	loc.23
	.data2	loc.24
	.data2	loc.25
	.data2	loc.26
	.data2	loc.27
	.data2	loc.28
	.data2	loc.29
	.data2	loc.30
	.data2	loc.31
	.data2	loc.32
	.data2	loc.33
	.data2	aar.2
	.data2	adf.s0
	.data2	adi.2
	.data2	adi.4
	.data2	adp.l
	.data2	adp.1
	.data2	adp.2
	.data2	adp.s0
	.data2	adp.sm1
	.data2	ads.2
	.data2	and.2
	.data2	asp.2
	.data2	asp.4
	.data2	asp.6
	.data2	asp.8
	.data2	asp.10
	.data2	asp.w0
	.data2	beq.l
	.data2	beq.s0
	.data2	bge.s0
	.data2	bgt.s0
	.data2	ble.s0
	.data2	blm.s0
	.data2	blt.s0
	.data2	bne.s0
	.data2	bra.l
	.data2	bra.sm1
	.data2	bra.sm2
	.data2	bra.s0
	.data2	bra.s1
	.data2	cal.1
	.data2	cal.2
	.data2	cal.3
	.data2	cal.4
	.data2	cal.5
	.data2	cal.6
	.data2	cal.7
	.data2	cal.8
	.data2	cal.9
	.data2	cal.10
	.data2	cal.11
	.data2	cal.12
	.data2	cal.13
	.data2	cal.14
	.data2	cal.15
	.data2	cal.16
	.data2	cal.17
	.data2	cal.18
	.data2	cal.19
	.data2	cal.20
	.data2	cal.21
	.data2	cal.22
	.data2	cal.23
	.data2	cal.24
	.data2	cal.25
	.data2	cal.26
	.data2	cal.27
	.data2	cal.28
	.data2	cal.s0
	.data2	cff.z
	.data2	cif.z
	.data2	cii.z
	.data2	cmf.s0
	.data2	cmi.2
	.data2	cmi.4
	.data2	cmp.z
	.data2	cms.s0
	.data2	csa.2
	.data2	csb.2
	.data2	dec.z
	.data2	dee.w0
	.data2	del.wm1
	.data2	dup.2
	.data2	dvf.s0
	.data2	dvi.2
	.data2	fil.l
	.data2	inc.z
	.data2	ine.l
	.data2	ine.w0
	.data2	inl.m2
	.data2	inl.m4
	.data2	inl.m6
	.data2	inl.wm1
	.data2	inn.s0
	.data2	ior.2
	.data2	ior.s0
	.data2	lae.l
	.data2	lae.w0
	.data2	lae.w1
	.data2	lae.w2
	.data2	lae.w3
	.data2	lae.w4
	.data2	lae.w5
	.data2	lae.w6
	.data2	lal.p
	.data2	lal.n
	.data2	lal.0
	.data2	lal.m1
	.data2	lal.w0
	.data2	lal.wm1
	.data2	lal.wm2
	.data2	lar.2
	.data2	ldc.0
	.data2	lde.l
	.data2	lde.w0
	.data2	ldl.0
	.data2	ldl.wm1
	.data2	lfr.2
	.data2	lfr.4
	.data2	lfr.s0
	.data2	lil.wm1
	.data2	lil.w0
	.data2	lil.0
	.data2	lil.2
	.data2	lin.l
	.data2	lin.s0
	.data2	lni.z
	.data2	loc.l
	.data2	loc.m1
	.data2	loc.s0
	.data2	loc.sm1
	.data2	loe.l
	.data2	loe.w0
	.data2	loe.w1
	.data2	loe.w2
	.data2	loe.w3
	.data2	loe.w4
	.data2	lof.l
	.data2	lof.2
	.data2	lof.4
	.data2	lof.6
	.data2	lof.8
	.data2	lof.s0
	.data2	loi.l
	.data2	loi.1
	.data2	loi.2
	.data2	loi.4
	.data2	loi.6
	.data2	loi.8
	.data2	loi.s0
	.data2	lol.p
	.data2	lol.n
	.data2	lol.0
	.data2	lol.2
	.data2	lol.4
	.data2	lol.6
	.data2	lol.m2
	.data2	lol.m4
	.data2	lol.m6
	.data2	lol.m8
	.data2	lol.m10
	.data2	lol.m12
	.data2	lol.m14
	.data2	lol.m16
	.data2	lol.w0
	.data2	lol.wm1
	.data2	lxa.1
	.data2	lxl.1
	.data2	lxl.2
	.data2	mlf.s0
	.data2	mli.2
	.data2	mli.4
	.data2	rck.2
	.data2	ret.0
	.data2	ret.2
	.data2	ret.s0
	.data2	rmi.2
	.data2	sar.2
	.data2	sbf.s0
	.data2	sbi.2
	.data2	sbi.4
	.data2	sdl.wm1
	.data2	set.s0
	.data2	sil.wm1
	.data2	sil.w0
	.data2	sli.2
	.data2	ste.l
	.data2	ste.w0
	.data2	ste.w1
	.data2	ste.w2
	.data2	stf.l
	.data2	stf.2
	.data2	stf.4
	.data2	stf.s0
	.data2	sti.1
	.data2	sti.2
	.data2	sti.4
	.data2	sti.6
	.data2	sti.8
	.data2	sti.s0
	.data2	stl.p
	.data2	stl.n
	.data2	stl.p0
	.data2	stl.p2
	.data2	stl.m2
	.data2	stl.m4
	.data2	stl.m6
	.data2	stl.m8
	.data2	stl.m10
	.data2	stl.wm1
	.data2	teq.z
	.data2	tgt.z
	.data2	tlt.z
	.data2	tne.z
	.data2	zeq.l
	.data2	zeq.s0
	.data2	zeq.s1
	.data2	zer.s0
	.data2	zge.s0
	.data2	zgt.s0
	.data2	zle.s0
	.data2	zlt.s0
	.data2	zne.s0
	.data2	zne.sm1
	.data2	zre.l
	.data2	zre.w0
	.data2	zrl.m2
	.data2	zrl.m4
	.data2	zrl.wm1
	.data2	zrl.n
	.data2	loop1
	.data2	loop2

!----------------- END OF MAIN DISPATCH -------------------------------

xxx:
	.data2	loc.0
	.data2	loc.1
init:
	ld sp,(bdos+1)	! address of fbase
	ld hl,xxx
	ld de,dispat
	ld bc,4
	ldir
	call uxinit
warmstart:
	ld sp,(bdos+1)	! address of fbase
	call makeargv
	ld de,0x80
	ld c,setdma
	call bdos
	ld c,open
	ld de,fcb
	call bdos
	inc a
	jr z,bademfile
	ld c,read
	ld de,fcb
	call bdos
	or a
	jr nz,bademfile	! no file
	ld de,header
	ld hl,0x90	! start of 2nd half of header
	ld bc,10	! we copy only first 5 words
	ldir
	ld de,(ntext)	! size of program text in bytes
	ld hl,0
	sbc hl,de
	add hl,sp
	ld sp,hl	! save space for program
	ld (pb),hl	! set procedure base
	ld a,0xa0
	ld (nextp),a
	ld de,(ntext)
	xor a
	ld h,a
	ld l,a
	sbc hl,de
	ex de,hl
	ld h,a
	ld l,a
	add hl,sp
1:	call getb
	ld (hl),c
	inc hl
	inc e
	jr nz,1b
	inc d
	jr nz,1b
			! now program text has been read,so start read-
	ld iy,0		! ing data descriptors, (nextp) (was hl) is
	ld ix,eb	! pointer into DMA,ix is pointer into global
	! data area,iy is #bytes pushed in last instr (used for repeat)
rddata:	ld hl,(ndata)
	ld a,h
	or l
	jr z,prdes	! no data left
	dec hl
	ld (ndata),hl
	call getb	! read 1 byte (here:init type) into register c
	dec c
	jp p,2f
	call getw
	push iy
	pop hl
	ld a,h
	or l
	jr z,5f		! size of block is zero, so no work
	push hl
	push bc
3:	pop hl		! #repeats
	pop bc		! block size
	push bc
	ld a,h
	or l
	jr z,4f		! ready
	dec hl
	push hl
	push ix
	pop hl
	add ix,bc
	dec hl
	ld d,h
	ld e,l
	add hl,bc
	ex de,hl
	lddr
	jr 3b
4:	pop bc
5:	ld iy,0		! now last instruction = repeat = type 0
	jr rddata
2:	ld b,c		! here other types come
	jr nz,2f	! Z-flag was (re-)set when decrementing c
	call getb	! uninitialized words, fetch #words
	sla c
	rl b
	ld iy,0
	add iy,bc
	add ix,bc
4:	jr rddata
2:	call getb	! remaining types, first fetch #bytes/words
	ld a,b
	cp 7
	jr z,rdflt
	jp p,bademfile	! floats are not accepted,nor are illegal types
	ld b,0
	cp 1
	jr z,2f
	cp 5
	jp m,1f
2:	ld iy,0		! initialized bytes, simply copy from EM-1 file
	add iy,bc
	ld b,c		! #bytes
3:
	call getb
	ld (ix),c
	inc ix
	djnz 3b
	jr 4b
1:	cp 2
	jr z,2f
	cp 3
	jr z,3f
	ld hl,(pb)
	jr 4f
3:	ld hl,eb
	jr 4f
2:	ld hl,0
4:	ld (ntext),hl	! ntext is used here to hold base address of
	ld iy,0		! correct type: data,instr or 0 (plain numbers)
	add iy,bc
	add iy,bc
	ld b,c
1:
	push bc
	ex de,hl	! save e into l
	call getw
	ex de,hl
	ld hl,(ntext)
	add hl,bc
	ld (ix),l
	inc ix
	ld (ix),h
	inc ix
	pop bc
	djnz 1b
2:	jr rddata
rdflt:
	ld a,c
	cp 4
	jr nz,bademfile
	push ix
	pop hl
1:	call getb
	ld a,c
	ld (hl),a
	inc hl
	or a
	jr nz,1b
	push ix
	pop hl
	call atof
	ld b,4
1:	ld a,(hl)
	ld (ix),a
	inc ix
	inc hl
	djnz 1b
	jr rddata

bademfile:
	ld c,printstring
	ld de,1f
	call bdos
	jp 0
1:	.ascii 'load file error\r\n$'

! now all data has been read,so on with the procedure descriptors
prdes:
	ld (hp),ix	! initialize heap pointer
	ld de,(nproc)
	ld hl,0
	xor a
	sbc hl,de
	add hl,hl
	add hl,hl	! 4 bytes per proc-descriptor
	add hl,sp
	ld sp,hl	! save space for procedure descriptors
	push hl
	pop ix
	ld (pd),hl	! initialize base
	ld hl,(nproc)
1:	ld a,h
	or l
	jr z,2f
	dec hl
	call getb
	ld (ix),c
	inc ix
	call getb
	ld (ix),c
	inc ix
	call getw
	ex de,hl
	ld hl,(pb)
	add hl,bc
	ld (ix),l
	inc ix
	ld (ix),h
	inc ix
	ex de,hl
	jr 1b
2:
	ld de,(entry)	! get ready for start of program
	ld ix,0		! reta, jumping here will stop execution
	push ix
	ld hl,argv
	push hl
	ld hl,(argc)
	push hl
	jr cal		! call EM-1 main program

getw:	call getb
	ld b,c
	call getb
	ld a,b
	ld b,c
	ld c,a
	ret
getb:	push hl		! getb reads 1 byte in register c from standard
	push de
	ld a,(nextp)	! DMA buffer and refills if necessary
	or a
	jr nz,1f
	push bc
	ld c,read
	ld de,fcb
	call bdos
	or a
	jr nz,bademfile
	pop bc
	ld a,0x80
1:	ld l,a
	ld h,0
	ld c,(hl)
	inc a
	ld (nextp),a
	pop de
	pop hl
	ret

!------------------------- Main loop of the interpreter ---------------

phl:	push hl
loop:	ld e,(ix)	! e = opcode byte
	inc ix		! advance EM program counter to next byte
	ld hl,dispat	! hl = address of dispatching table
	xor a
	ld d,a
	add hl,de	! compute address of routine for this opcode
	add hl,de	! hl = address of routine to dispatch to
	ld d,(hl)	! e = low byte of routine address
	inc hl		! hl now points to 2nd byte of routine address
	ld h,(hl)	! h = high byte of routine address
	ld l,d		! hl = address of routine
	ld d,a
	jp (hl)		! go execute the routine

loop1:	ld e,(ix)	! e = opcode byte
	inc ix		! advance EM program counter to next byte
	ld hl,dispat1	! hl = address of dispatching table
	xor a
	ld d,a
	add hl,de	! compute address of routine for this opcode
	add hl,de	! hl = address of routine to dispatch to
	ld d,(hl)	! e = low byte of routine address
	inc hl		! hl now points to 2nd byte of routine address
	ld h,(hl)	! h = high byte of routine address
	ld l,d		! hl = address of routine
	ld d,a
	jp (hl)		! go execute the routine

loop2:	ld e,(ix)	! e = opcode byte
	inc ix		! advance EM program counter to next byte
	ld hl,dispat2	! hl = address of dispatching table
	xor a
	ld d,a
	add hl,de	! compute address of routine for this opcode
	add hl,de	! hl = address of routine to dispatch to
	ld d,(hl)	! e = low byte of routine address
	inc hl		! hl now points to 2nd byte of routine address
	ld h,(hl)	! h = high byte of routine address
	ld l,d		! hl = address of routine
	ld d,a
	jp (hl)		! go execute the routine

! Note that d and a are both still 0, and the carry bit is cleared.
! The execution routines make heavy use of these properties.
! The reason that the carry bit is cleared is a little subtle, since the
! two instructions add hl,de affect it.  However, since dispat is being
! added twice a number < 256, no carry can occur.



!---------------------- Routines to compute addresses of locals -------

! There are four addressing routines, corresponding to four ways the
! offset can be represented:
! loml: 16-bit offset.  Codes 1-32767 mean offsets -2 to -65534 bytes
! loms:  8-bit offset.  Codes 1-255   mean offsets -2 to   -510 bytes
! lopl: 16-bit offset.  Codes 0-32767 mean offsets  0 to +65534 bytes
! lops:  8-bit offset.  Codes 0-255   mean offsets  0 to   +510 bytes

loml:	ld d,(ix)	! loml is for 16-bit offsets with implied minus
	inc ix
	jr 1f
loms:
	dec d
1:	ld e,(ix)	! loms is for 8-bit offsets with implied minus
	inc ix
	ld h,b
	ld l,c		! hl = bc
	add hl,de
	add hl,de	! hl now equals lb - byte offset
	jp (iy)

lopl:	ld d,(ix)	! lopl is for 16-bit offsets >= 0
	inc ix
lops:	ld h,d
	ld l,(ix)	! fetch low order byte of offset
	inc ix
	add hl,hl	! convert offset to bytes
	ld de,zone	! to account of return address zone
	add hl,de
	add hl,bc	! hl now equals lb - byte offset
	jp (iy)



!---------------------------- LOADS -----------------------------------

! LOC, LPI
loc.l: lpi.l:
	ld d,(ix)	! loc with 16-bit offset
	inc ix
loc.s0:	ld e,(ix)	! loc with 8-bit offset
	inc ix
loc.0:  loc.1:  loc.2:  loc.3:  loc.4:  loc.5:  loc.6:  loc.7: 
loc.8:  loc.9:  loc.10: loc.11: loc.12: loc.13: loc.14: loc.15:
loc.16: loc.17: loc.18: loc.19: loc.20: loc.21: loc.22: loc.23:
loc.24: loc.25: loc.26: loc.27: loc.28: loc.29: loc.30: loc.31:
loc.32: loc.33:
	push de
	jr loop

loc.m1:	ld hl,-1
	jr phl


loc.sm1:dec d		! for constants -256...-1
	jr loc.s0


! LDC
ldc.f:	ld h,(ix)
	inc ix
	ld l,(ix)
	inc ix
	push hl
	ld h,(ix)
	inc ix
	ld l,(ix)
	inc ix
	jr phl
ldc.l:	ld h,(ix)
	inc ix
	ld l,(ix)
	inc ix
	ld e,d
	bit 7,h
	jr z,1f
	dec de
1:
	push de
	jr phl

ldc.0:	ld e,d
	push de
	push de
	jr loop


! LOL

lol.0: lol.1: lol.2: lol.3: lol.4: lol.5: lol.6:
	ld hl,-b_lolp-b_lolp+zone
	add hl,de
	add hl,de
	add hl,bc
	jr ipsh

lol.m2: lol.m4: lol.m6: lol.m8: lol.m10: lol.m12: lol.m14: lol.m16:
	ld hl,b_loln+b_loln
	sbc hl,de
	xor a		! clear carry bit
	sbc hl,de
	add hl,bc	! hl = lb - byte offset

ipsh:	ld e,(hl)
	inc hl
	ld d,(hl)
	push de
	jr loop

lol.wm1:ld iy,ipsh
	jr loms
lol.n:	ld iy,ipsh
	jr loml
lol.w0:	ld iy,ipsh
	jr lops
lol.p:	ld iy,ipsh
	jr lopl


! LOE

loe.w4:	inc d
loe.w3:	inc d
loe.w2:	inc d
loe.w1:	inc d
loe.w0:	ld e,(ix)
	inc ix
	ld hl,eb
	add hl,de
	add hl,de
	jr ipsh

loe.l:	ld d,(ix)
	inc ix
	jr loe.w0



! LOF
lof.2: lof.4: lof.6: lof.8:
	ld hl,-b_lof-b_lof	! assume lof 1 means stack +2, not -2
	add hl,de
	add hl,de
 1:	pop de
	add hl,de
	jr ipsh

lof.s0:	ld h,d
 2:	ld l,(ix)
	inc ix
	jr 1b

lof.l:	ld h,(ix)
	inc ix
	jr 2b



! LAL
lal.m1:	ld h,b
	ld l,c
	dec hl
	jr phl
lal.0:	ld h,b
	ld l,c
	ld de,zone
	add hl,de
	jr phl

lal.wm2:dec d
lal.wm1:ld iy,phl
	jr loms
lal.w0:	ld iy,phl
	jr lops
lal.n:	ld h,(ix)
	inc ix
	ld l,(ix)
	inc ix
	add hl,bc
	jr phl

lal.p:	ld h,(ix)
	inc ix
	ld l,(ix)
	inc ix
	add hl,bc
	ld de,zone
	add hl,de
	jr phl



! LAE

lae.w8:	inc d
lae.w7: inc d
lae.w6: inc d
lae.w5: inc d
lae.w4: inc d
lae.w3: inc d
lae.w2: inc d
lae.w1: inc d
lae.w0: ld e,(ix)
	inc ix
	ld hl,eb
	add hl,de
	add hl,de
	jr phl

lae.l:	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	ld hl,eb
	add hl,de
	jr phl



! LIL
lil.0: lil.2:
	ld hl,-b_lil-b_lil+zone
	add hl,de
	add hl,de
	add hl,bc
 1:	ld e,(hl)
	inc hl
	ld h,(hl)
	ld l,e
	jr ipsh

lil.wm1:ld iy,1b
	jr loms
lil.n:	ld iy,1b
	jr loml
lil.w0:	ld iy,1b
	jr lops
lil.p:	ld iy,1b
	jr lopl



! LXL, LXA
lxl.1:
	ld a,1
	jr 7f

lxl.2:
	ld a,2
	jr 7f

lxl.l:	ld d,(ix)
	inc ix
lxl.s:	ld a,(ix)
	inc ix
7:	ld iy,phl
5:	ld h,b
	ld l,c
	or a
	jr z,3f
2:	inc hl
	inc hl
	inc hl
	inc hl
	inc hl
	inc hl
	inc hl
	inc hl
	.assert [ .-2b-zone] == 0
	ld e,(hl)
	inc hl
	ld h,(hl)
	ld l,e
	dec a
	jr nz,2b
3:	cp d
	jr z,4f
	dec d
	jr 2b
4:	jp (iy)

lxa.1:
	ld a,1
	jr 7f

lxa.l:	ld d,(ix)
	inc ix
lxa.s:	ld a,(ix)
	inc ix
7:	ld iy,1f
	jr 5b
1:	ld de,zone
	add hl,de
	jr phl

lpb.z:
	pop hl
	.assert [ zone/256] == 0
	ld e,zone
	add hl,de
	jr phl

dch.z:
	ld e,2
	jr loi

exg.z:
	pop de
	jr exg
exg.l:
	ld d,(ix)
	inc ix
exg.s0:
	ld e,(ix)
	inc ix
exg:
	push bc
	pop iy
	ld hl,0
	add hl,sp
	ld b,h
	ld c,l
	add hl,de
1:
	ld a,(bc)
	ex af,af2
	ld a,(hl)
	ld (bc),a
	ex af,af2
	ld (hl),a
	inc bc
	inc hl
	dec de
	ld a,d
	or e
	jr nz,1b
	push iy
	pop bc
	jr loop


! LDL
ldl.0:	ld de,zone
	ld h,b
	ld l,c
	add hl,de
dipsh:	inc hl
	inc hl
	inc hl
	ld d,(hl)
	dec hl
	ld e,(hl)
	dec hl
	push de
	ld d,(hl)
	dec hl
	ld e,(hl)
	push de
	jr loop

ldl.wm1:ld iy,dipsh
	jr loms
ldl.n:	ld iy,dipsh
	jr loml
ldl.w0:	ld iy,dipsh
	jr lops
ldl.p:	ld iy,dipsh
	jr lopl


! LDE
lde.l:	ld d,(ix)
	inc ix
	jr lde.w0

lde.w3:	inc d
lde.w2:	inc d
lde.w1:	inc d
lde.w0:	ld e,(ix)
	inc ix
	ld hl,eb
	add hl,de
	add hl,de
	jr dipsh


! LDF
ldf.l:	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	pop hl
	add hl,de
	jr dipsh


! LOI,LOS
los.z:
	ld iy,los.2
	jr pop2
los.l:	call long2
los.2:	pop de
loi:	pop hl
	add hl,de
	dec hl
	srl d
	rr e
	jr nc,1f
	ld a,e
	or d
	jr nz,eilsize
	ld e,(hl)	! here the 1-byte case is caught
	push de
	jr loop
1:	push bc
	pop iy
2:	ld b,(hl)
	dec hl
	ld c,(hl)
	dec hl
	push bc
	dec de
	ld a,d
	or e
	jr nz,2b
loiend:	push iy
	pop bc
	jr loop

loi.1: loi.2: loi.4: loi.6: loi.8:
	ld hl,-b_loi-b_loi
	add hl,de
	adc hl,de	! again we use that the carry is cleared
	jr nz,1f
	inc hl		! in case loi.0 object size is 1 byte!
1:	ex de,hl
	jr loi

loi.l:	ld d,(ix)
	inc ix
loi.s0:	ld e,(ix)
	inc ix
	jr loi


! ------------------------------ STORES --------------------------------

! STL
stl.p2:	ld hl,2
	jr 4f
stl.p0:	ld hl,0
4:	ld de,zone
	add hl,de
	add hl,bc
	jr ipop

stl.m2: stl.m4: stl.m6: stl.m8: stl.m10:
	ld hl,b_stlm+b_stlm
stl.zrl:sbc hl,de
	xor a
	sbc hl,de
	add hl,bc
ipop:	pop de
	ld (hl),e
	inc hl
	ld (hl),d
	jr loop

stl.wm1:ld iy,ipop
	jr loms
stl.n:	ld iy,ipop
	jr loml
stl.w0:	ld iy,ipop
	jr lops
stl.p:	ld iy,ipop
	jr lopl




! STE

ste.w3:	inc d
ste.w2:	inc d
ste.w1:	inc d
ste.w0:	ld e,(ix)
	inc ix
	ld hl,eb
	add hl,de
	add hl,de
	jr ipop

ste.l:	ld d,(ix)
	inc ix
	jr ste.w0



! STF
stf.2: stf.4: stf.6: 
	ld hl,-b_stf-b_stf
	add hl,de
	add hl,de
 1:	pop de
	add hl,de
	jr ipop

stf.s0:	ld h,d
 2:	ld l,(ix)
	inc ix
	jr 1b

stf.l:	ld h,(ix)
	inc ix
	jr 2b



! SIL
1:	ld e,(hl)
	inc hl
	ld h,(hl)
	ld l,e
	jr ipop

sil.wm1:ld iy,1b
	jr loms
sil.n:	ld iy,1b
	jr loml
sil.w0:	ld iy,1b
	jr lops
sil.p:	ld iy,1b
	jr lopl


! STI, STS
sts.z:
	ld iy,sts.2
	jr pop2
sts.l:	call long2
sts.2:	pop de
sti:	pop hl
	srl d
	rr e
	jr nc,1f
	ld a,e
	or d
	jr nz,eilsize
	pop de		! here the 1-byte case is caught
	ld (hl),e
	jr loop
1:	push bc
	pop iy
2:	pop bc
	ld (hl),c
	inc hl
	ld (hl),b
	inc hl
	dec de
	ld a,e
	or d
	jr nz,2b
	jr loiend

sti.1: sti.2: sti.4: sti.6: sti.8:
	ld hl,-b_sti-b_sti
	add hl,de
	adc hl,de	! again we use that the carry is cleared
	jr nz,1f
	inc hl		! in case sti.0 object size is 1 byte!
1:	ex de,hl
	jr sti

sti.l:	ld d,(ix)
	inc ix
sti.s0:	ld e,(ix)
	inc ix
	jr  sti


! SDL
sdl.wm1:ld iy,1f
	jr loms
sdl.n:	ld iy,1f
	jr loml
sdl.w0:	ld iy,1f
	jr lops
sdl.p:	ld iy,1f
	jr lopl


! SDE
sde.l:	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	ld hl,eb
2:	add hl,de
1:	pop de
	ld (hl),e
	inc hl
	ld (hl),d
	inc hl
	jr ipop


! SDF
sdf.l:	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	pop hl
	jr 2b


!------------------------- SINGLE PRECISION ARITHMETIC ---------------

! ADI, ADP, ADS, ADU

adi.z: adu.z:
	pop de
9:
	call chk24
	.data2 adi.2,adi.4
adi.l: adu.l:
	ld d,(ix)	! I guess a routine chk24.l could do this job
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
ads.z:
	ld iy,adi.2
	jr pop2
ads.l:
	call long2
ads.2: adi.2: adu.2:
	pop de
1:	pop hl
	add hl,de
	jr phl

adp.l:	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 1b

adp.sm1:dec d
adp.s0:	ld e,(ix)
	inc ix
	jr 1b

adp.2:	pop hl
	inc hl
	jr 1f
inc.z:
adp.1:	pop hl
1:	inc hl
	jr phl


! SBI, SBP, SBS, SBU	(but what is SBP?)

sbi.z: sbu.z:
	pop de
9:
	call chk24
	.data2 sbi.2,sbi.4
sbi.l: sbu.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
sbs.z:
	ld iy,sbi.2
	jr pop2
sbs.l:
	call long2
sbi.2:
	pop de
	pop hl
	sbc hl,de
	jr phl


! NGI
ngi.z:
	pop de
9:
	call chk24
	.data2 ngi.2,ngi.4
ngi.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
ngi.2:	ld hl,0
	pop de
	sbc hl,de
	jr phl


! MLI, MLU	Johan version
mli.z: mlu.z:
	pop de
9:
	call chk24
	.data2 mli.2,mli.4
mli.l: mlu.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
mli.2: mlu.2:
	ld iy,loop
mliint:	pop de
	pop hl
	push bc
	ld b,h
	ld c,l
	ld hl,0
	ld a,16
0:
	bit 7,d
	jr z,1f
	add hl,bc
1:
	dec a
	jr z,2f
	ex de,hl
	add hl,hl
	ex de,hl
	add hl,hl
	jr 0b
2:
	pop bc
	push hl
	jp (iy)


! DVI, DVU
dvi.z:	
	pop de
9:
	call chk24
	.data2 dvi.2,dvi.4
dvi.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
dvi.2:
	pop	hl
	pop	de
	push	bc
	ld	b,h
	ld	c,l
	xor	a
	ld	h,a
	ld	l,a
	sbc	hl,bc
	jp	m,1f
	ld	b,h
	ld	c,l
	cpl
1:
	or	a
	ld	hl,0
	sbc	hl,de
	jp	m,1f
	ex	de,hl
	cpl
1:
	push	af
	ld	hl,0
	ld	a,16
0:
	add	hl,hl
	ex	de,hl
	add	hl,hl
	ex	de,hl
	jr	nc,1f
	inc	hl
	or	a
1:
	sbc	hl,bc
	inc	de
	jp	p,2f
	add	hl,bc
	dec	de
2:
	dec	a
	jr	nz,0b
	pop	af
	or	a
	jr	z,1f
	ld	hl,0
	sbc	hl,de
	ex	de,hl
1:
	pop	bc
	push	de
	jr	loop


dvu.z:	
	pop de
9:
	call chk24
	.data2 dvu.2,dvu.4
dvu.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
dvu.2:
	pop	hl
	pop	de
	push	bc
	ld	b,h
	ld	c,l
	ld	hl,0
	ld	a,16
0:
	add	hl,hl
	ex	de,hl
	add	hl,hl
	ex	de,hl
	jr	nc,1f
	inc	hl
	or	a
1:
	sbc	hl,bc
	inc	de
	jp	p,2f
	add	hl,bc
	dec	de
2:
	dec	a
	jr	nz,0b
	pop	bc
	push	de
	jr loop


! RMI, RMU
rmi.z:
	pop de
9:
	call chk24
	.data2 rmi.2,rmi.4
rmi.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
rmi.2:
	pop	hl
	pop	de
	push	bc
	ld	b,h
	ld	c,l
	xor	a
	ld	h,a
	ld	l,a
	sbc	hl,bc
	jp	m,1f
	ld	b,h
	ld	c,l
1:
	or	a
	ld	hl,0
	sbc	hl,de
	jp	m,1f
	ex	de,hl
	cpl
1:
	push	af
	ld	hl,0
	ld	a,16
0:
	add	hl,hl
	ex	de,hl
	add	hl,hl
	ex	de,hl
	jr	nc,1f
	inc	hl
	or	a
1:
	sbc	hl,bc
	inc	de
	jp	p,2f
	add	hl,bc
	dec	de
2:
	dec	a
	jr	nz,0b
	ex	de,hl
	pop	af
	or	a
	jr	z,1f
	ld	hl,0
	sbc	hl,de
	ex	de,hl
1:
	pop	bc
	push	de
	jr	loop


rmu.4:
	ld iy,.dvu4
	jr 1f
rmi.4:
	ld iy,.dvi4
1:
	ld (retarea),bc
	ld (retarea+2),ix
	ld hl,1f
	push hl
	push iy
	ret
1:
	pop hl
	pop hl
	push bc
	push de
	ld bc,(retarea)
	ld ix,(retarea+2)
	jr loop
rmu.z:
	pop de
9:
	call chk24
	.data2 rmu.2,rmu.4
rmu.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
rmu.2:
	pop	hl
	pop	de
	push	bc
	ld	b,h
	ld	c,l
	ld	hl,0
	ld	a,16
0:
	add	hl,hl
	ex	de,hl
	add	hl,hl
	ex	de,hl
	jr	nc,1f
	inc	hl
	or	a
1:
	sbc	hl,bc
	inc	de
	jp	p,2f
	add	hl,bc
	dec	de
2:
	dec	a
	jr	nz,0b
	pop	bc
	jr	phl

! SLI, SLU

slu.z: sli.z:
	pop de
9:
	call chk24
	.data2 sli.2,sli.4
slu.l:
sli.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
sli.2:
	pop de
	pop hl
	ld a,d
	or a
	jr z,1f
	ld e,15
2:	add hl,hl
1:	dec e
	jp m,phl
	jr 2b

sli.4:
slu.4:
	pop de
	pop iy
	pop hl
	inc d
	dec d
	jr z,1f
	ld e,31
1:
	dec e
	jp m,2f
	add iy,iy
	adc hl,hl
	jr 1b
2:
	push hl
	push iy
	jr loop

! SRI, SRU

sri.z:
	pop de
9:
	call chk24
	.data2 sri.2,sri.4
sri.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
sri.2:	pop de
	pop hl
	ld a,d
	or a
	jr z,1f
	ld e,15
2:	sra h
	rr l
1:	dec e
	jp m,phl
	jr 2b


sri.4:
	pop de
	ld a,e
	inc d
	dec d
	pop de
	pop hl
	jr z,1f
	ld a,31
1:
	dec a
	jp m,2f
	sra h
	rr l
	rr d
	rr e
	jr 1b
2:
	push hl
	push de
	jr loop

sru.z:
	pop de
9:
	call chk24
	.data2 sru.2,sru.4
sru.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
sru.2:	pop de
	pop hl
	ld a,d
	or a
	jr z,1f
	ld e,15
2:	srl h
	rr l
1:	dec e
	jp m,phl
	jr 2b

sru.4:
	pop de
	ld a,e
	inc d
	dec d
	pop de
	pop hl
	jr z,1f
	ld a,31
1:
	dec a
	jp m,2f
	srl h
	rr l
	rr d
	rr e
	jr 1b
2:
	push hl
	push de
	jr loop

! ROL, ROR
rol.z:
	pop de
9:
	call chk24
	.data2 rol.2,rol.4
rol.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
rol.2:	pop de
	pop hl
	ld a,e
	and 15
	jr z,phl
	ld de,0
1:	add hl,hl
	adc hl,de
	dec a
	jr nz,1b
	jr phl


rol.4:
	pop de
	pop iy
	pop hl
	ld a,e
	and 31
	jr z,3f
1:
	add iy,iy
	adc hl,hl
	jr nc,2f
	inc iy
2:
	dec a
	jr nz,1b
3:
	push hl
	push iy

ror.z:
	pop de
9:
	call chk24
	.data2 ror.2,ror.4
ror.l:
	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr 9b
ror.2:	pop de
	pop hl
	ld a,e
	and 15
	jr z,phl
1:	srl h
	rr l
	jr nc,2f
	set 7,h
2:	dec a
	jr nz,1b
	jr phl


ror.4:
	pop de
	ld a,e
	pop de
	pop hl
	and 31
	jr z,0f
1:
	srl h
	rr l
	rr d
	rr e
	jr nc,2f
	set 7,h
2:
	dec a
	jr nz,1b
0:
	push hl
	push de
	jr loop
pop2:	ld de,2
	pop hl
	sbc hl,de
	jr nz,eilsize
	xor a
	ld d,a
	jp (iy)


chk24:
	! this routine is used to call indirectly
	! a routine for either 2 or 4 byte operation
	! ( e.g. mli.2 or mli.4)
	! de contains 2 or 4
	! iy points to a descriptor containing
	! the addresses of both routines
	pop iy		! address of descriptor
	ld a,d		! high byte must be 0
	or a
	jr nz,unimpld
	ld a,e
	cp 2
	jr z,1f
	inc iy
	inc iy		! points to word containing
			! address of 4 byte routine
	cp 4
	jr nz,unimpld
1:
	ld h,(iy+1)
	ld l,(iy)
	xor a
	jp (hl)
!--------------------- INCREMENT, DECREMENT, ZERO ----------------------

! INC
inl.m2: inl.m4: inl.m6:
	ld hl, b_inl+b_inl
	sbc hl,de
	xor a
	sbc hl,de
	add hl,bc
1:	inc (hl)
	jr nz,loop
	inc hl
	inc (hl)
	jr loop

inl.wm1:ld iy,1b
	jr loms
inl.n:	ld iy,1b
	jr loml
inl.p:	ld iy,1b
	jr lopl


! INE

ine.w3:	inc d
ine.w2:	inc d
ine.w1:	inc d
ine.w0:	ld e,(ix)
	inc ix
	ld hl,eb
	add hl,de
	add hl,de
	jr 1b

ine.l:	ld d,(ix)
	inc ix
	jr ine.w0


! DEC
dec.z:	pop hl
	dec hl
	push hl
	jr loop

1:	ld e,(hl)
	inc hl
	ld d,(hl)
	dec de
	ld (hl),d
	dec hl
	ld (hl),e
	jr loop

del.wm1:ld iy,1b
	jr loms
del.n:	ld iy,1b
	jr loml
del.p:	ld iy,1b
	jr lopl


! DEE

dee.w3:	inc d
dee.w2:	inc d
dee.w1:	inc d
dee.w0:	ld e,(ix)
	inc ix
	ld hl,eb
	add hl,de
	add hl,de
	jr 1b

dee.l:	ld d,(ix)
	inc ix
	jr dee.w0


! ZERO
zri2: zru2:
	ld h,d
	ld l,d
	jr phl


zrf.z:
zer.z:	pop de
2:	ld hl,0
	sra d
	rr e
1:	push hl
	dec de
	ld a,e
	or d
	jr nz,1b
	jr loop

zrf.l:
zer.l:	ld d,(ix)
	inc ix
zer.s0:	ld e,(ix)
	inc ix
	jr 2b


zrl.m2: zrl.m4:
	ld h,d
	ld l,d
	push hl
	ld hl,b_zrl+b_zrl
	jr stl.zrl

zrl.wm1:
	ld h,d
	ld l,d
	push hl
	jr stl.wm1

zrl.n:
	ld h,d
	ld l,d
	push hl
	jr stl.n

zrl.w0:
	ld h,d
	ld l,d
	push hl
	jr stl.w0

zrl.p:
	ld h,d
	ld l,d
	push hl
	jr stl.p



zre.w0:
	ld h,d
	ld l,d
	push hl
	jr ste.w0

zre.l:
	ld h,d
	ld l,d
	push hl
	jr ste.l


! ------------------------- CONVERT GROUP ------------------------------

! CII, CIU
cii.z: ciu.z:
	pop hl
	pop de
	sbc hl,de	! hl and de can only have values 2 or 4, that's
			! why a single subtract can split the 3 cases
	jr z,loop	! equal, so do nothing
	jp p,2f
3:	pop hl		! smaller, so shrink size from double to single
	pop de
	jr phl
2:	pop hl		! larger, so expand (for cii with sign extend)
	res 1,e
	bit 7,h
	jr z,1f
	dec de
1:	push de
	jr phl

! CUI, CUU
cui.z: cuu.z:
	pop hl
	pop de
	sbc hl,de
	jr z,loop
	jp m,3b
	res 1,e
	pop hl
	jr 1b


! ------------------------------ SETS ---------------------------------

! SET
set.z:	pop hl
doset:	pop de
	push bc
	pop iy
	ld b,h
	ld c,l
	xor a
0:	push af
	inc sp
	dec c
	jr nz,0b
	dec b
	jp p,0b
	push iy
	pop bc
	ex de,hl
	ld a,l
	sra h
	jp m,unimpld
	rr l
	sra h
	rr l
	sra h
	rr l
	push hl
	or a
	sbc hl,de
	pop hl
	jp p,unimpld
	add hl,sp
	ld (hl),1
	and 7
	jr 1f
0:	sla (hl)
	dec a
1:	jr nz,0b
	jr loop

set.l:	ld d,(ix)
	inc ix
set.s0: ld e,(ix)
	inc ix
	ex de,hl
	jr doset


! INN
inn.z:	pop hl
	jr 1f
inn.l:	ld d,(ix)
	inc ix
inn.s0: ld e,(ix)
	inc ix
	ex de,hl
1:
	pop de
	add hl,sp
	push hl
	pop iy
	ex de,hl
	ld a,l
	sra h
	jp m,0f
	rr l
	sra h
	rr l
	sra h
	rr l
	add hl,sp
	push hl
	or a		! clear carry
	sbc hl,de
	pop hl
	jp m,1f
0:	xor a
	jr 4f
1:	ld e,(hl)
	and 7
	jr 2f
3:	rrc e
	dec a
2:	jr nz,3b
	ld a,e
	and 1
4:	ld l,a
	ld h,0
	ld sp,iy
	jr phl



! ------------------------- LOGICAL GROUP -----------------------------

! AND
and.z:	pop de
doand:	ld h,d
	ld l,e
	add hl,sp
	push bc
	ld b,h
	ld c,l
	ex de,hl
	add hl,de
1:	dec hl
	dec de
	ld a,(de)
	and (hl)
	ld (hl),a
	xor a
	sbc hl,bc
	jr z,2f
	add hl,bc
	jr 1b
2:	ld h,b
	ld l,c
	pop bc
	ld sp,hl
	jr loop

and.l:	ld d,(ix)
	inc ix
and.s0:	ld e,(ix)
	inc ix
	jr doand

and.2:	ld e,2
	jr doand

! IOR
ior.z:	pop de
ior:	ld h,d
	ld l,e
	add hl,sp
	push bc
	ld b,h
	ld c,l
	ex de,hl
	add hl,de
1:	dec hl
	dec de
	ld a,(de)
	or (hl)
	ld (hl),a
	xor a
	sbc hl,bc
	jr z,2f
	add hl,bc
	jr 1b
2:	ld h,b
	ld l,c
	pop bc
	ld sp,hl
	jr loop

ior.l:	ld d,(ix)
	inc ix
ior.s0:	ld e,(ix)
	inc ix
	jr ior

ior.2:	ld e,2
	jr ior

! XOR
xor.z:	pop de
exor:	ld h,d
	ld l,e
	add hl,sp
	push bc
	ld b,h
	ld c,l
	ex de,hl
	add hl,de
1:	dec hl
	dec de
	ld a,(de)
	xor (hl)
	ld (hl),a
	xor a
	sbc hl,bc
	jr z,2f
	add hl,bc
	jr 1b
2:	ld h,b
	ld l,c
	pop bc
	ld sp,hl
	jr loop

xor.l:	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	jr exor

! COM
com.z:	pop hl
com:	add hl,sp
1:	dec hl
	ld a,(hl)
	cpl
	ld (hl),a
	xor a
	sbc hl,sp
	jr z,loop
	add hl,sp
	jr 1b

com.l:	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
	ex de,hl
	jr com


! ------------------------- COMPARE GROUP ------------------------------

! CMI


cmi.2:	pop de
	pop hl
	ld a,h
	xor d		! check sign bit to catch overflow with subtract
	jp m,1f
	sbc hl,de
	jr phl
1:	xor d		! now a equals (original) h again
	jp m,phl
	set 0,l		! to catch case hl=0>de bit 0 is set explicitly
	jr phl



! CMU, CMP

cmi.4:	inc a
	ld de,4
	jr docmu
cmp.z:	ld de,2
	jr docmu
cmi.z:	inc a
cmu.z:
	pop de
	jr docmu

cmi.l:	inc a
cmu.l:	ld d,(ix)
	inc ix
	ld e,(ix)
	inc ix
docmu:	push bc
	pop iy
	ld b,d
	ld c,e
	ld hl,0
	add hl,sp
	add hl,bc
	dec hl
	ld d,h
	ld e,l
	add hl,bc
	ld (retarea),hl		! save new sp-1
	or a
	jr z,1f
	ld a,(de)
	cp (hl)
	dec hl
	dec de
	dec bc
	jr z,1f
	jp p,4f
	jp pe,5f
	jr 6f
1:
	ld a,(de)
	cp (hl)
	dec de
	dec hl
	dec bc
	jr nz,2f
	ld a,b
	or c
	jr nz,1b
	ld d,a
	ld e,a
	jr 3f
2:
	jr nc,5f
6:
	ld de,1
	jr 3f
4:
	jp pe,6b
5:
	ld de,-1
3:
	ld hl,(retarea)
	inc hl
	ld sp,hl
	push de
	push iy
	pop bc
	jr loop



! CMS

cms.z:	pop hl
	jr 1f
cms.l:	ld d,(ix)
	inc ix
cms.s0:	ld e,(ix)
	inc ix
	ex de,hl
1:	push bc
	pop iy
	ld b,h
	ld c,l
	add hl,sp
0:
	dec sp
	pop af
	cpi
	jr nz,1f
	ld a,b
	or c
	jr nz,0b
	ld de,0
	jr 2f
1:
	add hl,bc
	ld de,1
2:
	ld sp,hl
	push de
	push iy
	pop bc
	jr loop


! TLT, TLE, TEQ, TNE, TGE, TGT
tlt.z:
	ld h,d
	ld l,d
	pop de
	bit 7,d
	jr z,1f
	inc l
1:
	jr phl

tle.z:	ld hl,1
	pop de
	xor a
	add a,d
	jp m,phl
	jr nz,1f
	xor a
	add a,e
	jr z,2f
1:	dec l
2:
	jr phl

teq.z:
	ld h,d
	ld l,d
	pop de
	ld a,d
	or e
	jr nz,1f
	inc l
1:
	jr phl

tne.z:
	ld h,d
	ld l,d
	pop de
	ld a,d
	or e
	jr z,1f
	inc l
1:
	jr phl

tge.z:
	ld h,d
	ld l,d
	pop de
	bit 7,d
	jr nz,1f
	inc l
1:
	jr phl

tgt.z:
	ld h,d
	ld l,d
	pop de
	xor a
	add a,d
	jp m,phl
	jr nz,1f
	xor a
	add a,e
	jr z,2f
1:	inc l
2:
	jr phl


! ------------------------- BRANCH GROUP -------------------------------

! BLT, BLE, BEQ, BNE, BGE, BGT, BRA

b.pl:	ld d,(ix)
	inc ix
b.ps:	ld e,(ix)
	inc ix
	push ix
	pop hl
	add hl,de
	pop de
	ex (sp),hl
	xor a
	jp (iy)


bra.l:	ld d,(ix)
	inc ix
	jr bra.s0

bra.sm2:dec d
bra.sm1:dec d
	dec d
bra.s1:	inc d
bra.s0:	ld e,(ix)
	inc ix
	add ix,de
	jr loop


bgo:	pop ix		! take branch
	jr loop


blt.s0:	ld iy,blt
	jr b.ps
blt.l:	ld iy,blt
	jr b.pl
blt:	ld a,h
	xor d
	jp m,1f
	sbc hl,de
	jr 2f
1:	xor d
2:	jp m,bgo
	pop de
	jr loop


ble.s0:	ld iy,ble
	jr b.ps
ble.l:	ld iy,ble
	jr b.pl
ble:	ex de,hl
	jr bge


beq.s0:	ld iy,beq
	jr b.ps
beq.l:	ld iy,beq
	jr b.pl
beq:	sbc hl,de
	jr z,bgo
	pop de		! keep stack clean, so dump unused jump address
	jr loop


bne.s0:	ld iy,bne
	jr b.ps
bne.l:	ld iy,bne
	jr b.pl
bne:	sbc hl,de
	jr nz,bgo
	pop de		! keep stack clean, so dump unused jump address
	jr loop


bge.s0:	ld iy,bge
	jr b.ps
bge.l:	ld iy,bge
	jr b.pl
bge:	ld a,h
	xor d		! check sign bit to catch overflow with subtract
	jp m,1f
	sbc hl,de
	jr 2f
1:	xor d		! now a equals (original) h again
2:	jp p,bgo
	pop de		! keep stack clean, so dump unused jump address
	jr loop


bgt.s0:	ld iy,bgt
	jr b.ps
bgt.l:	ld iy,bgt
	jr b.pl
bgt:	ex de,hl
	jr blt



! ZLT, ZLE, ZEQ, ZNE, ZGE, ZGT


z.pl:	ld d,(ix)
	inc ix
z.ps:	ld e,(ix)
	inc ix
	push ix
	pop hl
	add hl,de
	ex de,hl
	pop hl
	xor a
	add a,h
	jp (iy)



zlt.l:	ld iy,zlt
	jr z.pl
zlt.s0:	ld iy,zlt
	jr z.ps
zlt:	jp m,zgo
	jr loop


zle.l:	ld iy,zle
	jr z.pl
zle.s0:	ld iy,zle
	jr z.ps
zle:	jp m,zgo
	jr nz,loop
	xor a
	add a,l
	jr z,zgo
	jr loop


zeq.l:	ld iy,zeq
	jr z.pl
zeq.s1: inc d
zeq.s0:	ld iy,zeq
	jr z.ps
zeq:	ld a,l
	or h
	jr nz,loop
zgo:	push de
	pop ix
	jr loop


zne.sm1:dec d
	jr zne.s0
zne.l:	ld iy,zne
	jr z.pl
zne.s0:	ld iy,zne
	jr z.ps
zne:	ld a,l
	or h
	jr nz,zgo
	jr loop


zge.l:	ld iy,zge
	jr z.pl
zge.s0:	ld iy,zge
	jr z.ps
zge:	jp m,loop
	jr zgo


zgt.l:	ld iy,zgt
	jr z.pl
zgt.s0:	ld iy,zgt
	jr z.ps
zgt:	jp m,loop
	jr nz,zgo
	xor a
	add a,l
	jr z,loop
	jr zgo


! ------------------- ARRAY REFERENCE GROUP ---------------------------

! AAR
aar.z:
	ld iy,aar.2
	jr pop2
aar.l:	call long2
aar.2:	ld hl,loop
aarint:	pop iy		! descriptor
	ex (sp),hl	! save return address and hl:=index
	ld e,(iy+0)
	ld d,(iy+1)	! de := lwb
	ld a,h
	xor d
	jp m,1f
	sbc hl,de
	jr 2f
1:	sbc hl,de
	xor d
2:	call m,e.array
	ld e,(iy+2)
	ld d,(iy+3)	! de := upb - lwb
	push hl
	ex de,hl
	ld a,h
	xor d
	jp m,1f
	sbc hl,de
	jr 2f
1:	xor d
2:	ex de,hl
	pop hl
	call m,e.array
1:	ld e,(iy+4)
	ld d,(iy+5)
	pop iy
	ex (sp),iy
	push iy		! exchange base address and return address
	push de
	push de
	push hl
	ld iy,1f
	jr mliint
1:	pop de
	pop iy
	pop hl
	push iy
	add hl,de
	pop de
	ex (sp),hl
	jp (hl)

lar.l:	call long2
lar.2:	ld hl,loi
	jr aarint
lar.z:
	ld iy,lar.2
	jr pop2


sar.l:	call long2
sar.2:	ld hl,sti
	jr aarint
sar.z:
	ld iy,sar.2
	jr pop2


! --------------------- PROCEDURE CALL/RETURN --------------------------

! CAL

cal.1: cal.2: cal.3: cal.4: cal.5: cal.6: cal.7: cal.8:
cal.9: cal.10: cal.11: cal.12: cal.13: cal.14: cal.15: cal.16:
cal.17: cal.18: cal.19: cal.20: cal.21: cal.22: cal.23: cal.24:
cal.25: cal.26: cal.27: cal.28:
	ld hl,-b_cal
	add hl,de
	ex de,hl
	jr cal

cal.l:	ld d,(ix)
	inc ix
cal.s0:	ld e,(ix)
	inc ix
cal:	push ix		! entry point for main program of interpreter
	push bc
	ld hl,(eb)
	push hl
	ld hl,(ebp4)
	push hl
! temporary tracing facility
! NOP it if you don't want it
	push de
	ld de,(ebp4)
	ld hl,(eb)
	call prline
	pop de
! end of temporary tracing
	ld hl,0
	add hl,sp
	ld b,h
	ld c,l
	ld hl,(pd)
	ex de,hl
	add hl,hl
	add hl,hl
	add hl,de
	push hl
	pop iy
	ld e,(iy+0)
	ld d,(iy+1)
	ld l,c
	ld h,b
	xor a
	sbc hl,de
	ld sp,hl
	ld e,(iy+2)
	ld d,(iy+3)
	ld ix,0
	add ix,de
	jr loop


! CAI

cai.z:	pop de
	jr cal


! LFR
lfr.z:	pop de
2:	ld a,e
	rr a
	cp 5
	jp p,eilsize	! only result sizes <= 8 are allowed
	ld hl,retarea
	add hl,de
1:	dec hl
	ld d,(hl)
	dec hl
	ld e,(hl)
	push de
	dec a
	jr nz,1b
	jr loop

lfr.l:	ld d,(ix)
	inc ix
lfr.s0:	ld e,(ix)
	inc ix
	jr 2b

lfr.2:	ld hl,(retarea)
	jr phl

lfr.4:	ld de,4
	jr 2b


! RET
ret.2:	ld a,1
	jr 3f

ret.z:	pop de
2:	ld a,d
	or e
	jr z,ret.0
	rr a
	cp 5
	jp p,eilsize	! only result sizes <= 8 bytes are allowed
3:	ld hl,retarea
1:	pop de
	ld (hl),e
	inc hl
	ld (hl),d
	inc hl
	dec a
	jr nz,1b
ret.0:
	ld h,b
	ld l,c
	ld sp,hl
	pop hl
	ld (ebp4),hl
	pop hl
	ld (eb),hl
	pop bc		! old LB
	pop ix		! reta
	push ix		! check to see if reta = boot (= 0)
	pop hl
	ld a,l
	or h
	jr nz,loop	! not done yet
	call uxfinish
	jr boot

ret.l:	ld d,(ix)
	inc ix
ret.s0:	ld e,(ix)
	inc ix
	jr 2b


! ------------------------- MISCELLANEOUS -----------------------------

! SIG, TRP, RTT

sig.z:
	ld hl,(trapproc)
	ex (sp),hl
	ld (trapproc),hl
	jr loop

trp.z:
	ex (sp),hl
	push de
	push af
	push ix
	push iy
	push bc
!	ld iy,trapproc
!	ld a,(iy)
!	or (iy+1)
!	jr nz,1f
	ld iy,2f+13
	call octnr
	ld c,printstring
	ld de,2f
	call bdos
	ld de,(ebp4)
	ld hl,(eb)
	call prline
0:
	pop iy		! LB
	ld a,(iy+6)
	or (iy+7)	! reta
	jr nz,3f
	call uxfinish
	jp boot
3:
	ld c,(iy+4)
	ld b,(iy+5)
	push bc		! next LB
	ld e,(iy)
	ld d,(iy+1)	! file name
	ld l,(iy+2)
	ld h,(iy+3)	! lineno
	call prline
	jr 0b
!1:
!	ld ix,0
!	push hl
!	ld hl,(trapproc)
!	push hl
!	ld hl,0
!	ld (trapproc),hl
!	jr cai.z
2:	.ascii 'error 0xxxxxx\r\n$'

prline:
! prints lineno (hl) and filename (de)
	push de
	ld iy,2f+12
	call octnr
	ld c,printstring
	ld de,2f
	call bdos
	pop de
	ld hl,4f
0:
	ld a,(de)
	or a
	jr z,1f
	ld (hl),a
	inc de
	inc hl
	jr 0b
1:
	ld (hl),36	! '$'
	ld de,4f
	ld c,printstring
	call bdos
	ld de,3f
	ld c,printstring
	call bdos
	ret
2:	.ascii 'line 0xxxxxx in $'
3:	.ascii '\r\n$'
4:	.space 12

rtt.z=ret.0



! NOP
! changed into output routine to print linenumber
! in octal (6 digits)

nop.z:	push bc
	ld iy,1f+12
	ld hl,(eb)
	call octnr
	ld iy,1f+20
	ld hl,0
	add hl,sp
	call octnr
	ld c,printstring
	ld de,1f
	call bdos
	pop bc
	jr loop
1:	.ascii 'test 0xxxxxx 0xxxxxx\r\n$'

octnr:
	ld b,6
1:	ld a,7
	and l
	add a,'0'
	dec iy
	ld (iy+0),a
	srl h
	rr l
	srl h
	rr l
	srl h
	rr l
	djnz 1b
	ret


! DUP

dup.2:	pop hl
	push hl
	jr phl

dus.z:
	ld iy,1f
	jr pop2
dus.l:	call long2
1:	push bc
	pop iy
	pop bc
	jr dodup
dup.l:
	push bc
	pop iy
	ld b,(ix)
	inc ix
	ld c,(ix)
	inc ix
dodup:	ld h,d
	ld l,d		! ld hl,0
	add hl,sp
	ld d,h
	ld e,l
	xor a
	sbc hl,bc
	ld sp,hl
	ex de,hl
	ldir
	push iy
	pop bc
	jr loop


! BLM, BLS
bls.z:
	ld iy,blm
	jr pop2
bls.l:	call long2
blm:
	push bc
	pop iy
	pop bc
	pop de
	pop hl
	ldir
	push iy
	pop bc
	jr loop

blm.l:
	ld d,(ix)
	inc ix
blm.s0:	ld e,(ix)
	inc ix
	push de
	jr blm


! ASP, ASS
ass.z:
	ld iy,1f
	jr pop2
ass.l:	call long2
1:	pop hl
	jr 1f
asp.l:
	ld h,(ix)
	inc ix
	ld l,(ix)
	inc ix
asp:	add hl,hl
1:	add hl,sp
	ld sp,hl
	jr loop


asp.2: asp.4: asp.6: asp.8: asp.10:
	ld hl,-b_asp
	add hl,de
	jr asp

asp.w0:	ld e,(ix)
	inc ix
	ex de,hl
	jr asp


! CSA

csa.z:
	ld iy,csa.2
	jr pop2
csa.l:	call long2
csa.2:
!! temporary version while bug in cem remains
!	pop iy
!	pop de
!	push bc
!	ld c,(iy)
!	ld b,(iy+1)
!	ld l,(iy+4)
!	ld h,(iy+5)
!	xor a
!	sbc hl,de
!	jp m,1f
!	ex de,hl
!	ld e,(iy+2)
!	ld d,(iy+3)
!	xor a
!	sbc hl,de
!	jp m,1f
! end of temporary piece
	pop iy
	pop hl
	push bc
	ld c,(iy)
	ld b,(iy+1)
	ld e,(iy+2)
	ld d,(iy+3)
	xor a
	sbc hl,de
	jp m,1f
	ex de,hl
	ld l,(iy+4)
	ld h,(iy+5)
	xor a
	sbc hl,de
	jp m,1f
	ex de,hl
	add hl,hl
	ld de,6
	add hl,de
	ex de,hl
	add iy,de
	ld l,(iy)
	ld h,(iy+1)
	ld a,h
	or l
	jr nz,2f
1:	ld a,b
	or c
	jr z,e.case
	ld l,c
	ld h,b
2:	pop bc
	push hl
	pop ix
	jr loop
! CSB

csb.z:
	ld iy,csb.2
	jr pop2
csb.l:	call long2
csb.2:
	pop ix
	pop iy
	ld e,(ix)
	inc ix
	ld d,(ix)
	inc ix
	push de
	ex (sp),iy
	pop de
	push bc
	ld c,(ix)
	inc ix
	ld b,(ix)
	inc ix
1:
	ld a,b
	or c
	jr z,noteq
	ld a,(ix+0)
	cp e
	jr nz,2f
	ld a,(ix+1)
	cp d
	jr nz,2f
	ld l,(ix+2)
	ld h,(ix+3)
	jr 3f
2:	inc ix
	inc ix
	inc ix
	inc ix
	dec bc
	jr 1b
noteq:	push iy
	pop hl
3:	ld a,l
	or h
	jr z,e.case
2:
	pop bc
	push hl
	pop ix
	jr loop


! LIN
lin.l:	ld d,(ix)
	inc ix
lin.s0:	ld e,(ix)
	inc ix
	ld (eb),de
	jr loop


! FIL
fil.z:	pop hl
1:
	ld (ebp4),hl
	jr loop

fil.l:	ld h,(ix)
	inc ix
	ld l,(ix)
	inc ix
	ld de,eb
	add hl,de
	jr 1b


! LNI
lni.z:	ld hl,(eb)
	inc hl
	ld (eb),hl
	jr loop


! RCK
rck.z:
	ld iy,rck.2
	jr pop2
rck.l:	call long2
rck.2:
	pop iy
3:	pop hl
	push hl
	ld e,(iy)
	ld d,(iy+1)
	ld a,h
	xor d		! check sign bit to catch overflow with subtract
	jp m,1f
	sbc hl,de
	jr 2f
1:	xor d		! now a equals (original) h again
2:	call m,e.rck
	pop de
	push de
	ld l,(iy+2)
	ld h,(iy+3)
	ld a,h
	xor d		! check sign bit to catch overflow with subtract
	jp m,1f
	sbc hl,de
	jr 2f
1:	xor d		! now a equals (original) h again
2:	call m,e.rck
	jr loop


! LIM
lim.z:	ld hl,(ignmask)
	jr phl


! SIM
sim.z:	pop de
	ld (ignmask),de
	jr loop


! LOR

lor.s0:	ld e,(ix)
	inc ix
	ld a,d
	or e
	jr nz,1f
	push bc
	jr loop
1:	ld hl,-1
	adc hl,de
	jr nz,1f
	add hl,sp
	jr phl
1:	ld hl,(hp)
	jr phl


! STR

str.s0:	ld e,(ix)
	inc ix
	ld a,d
	or e
	jr nz,1f
	pop bc
	jr loop
1:	pop hl
	dec de
	ld a,d
	or e
	jr nz,1f
	ld sp,hl
	jr loop
1:	ld (hp),hl
	jr loop

! Floating point calling routines

loadfregs:
	pop hl
	pop de
	ld (fpac),de
	pop de
	ld (fpac+2),de
	pop de
	ld (fpop),de
	pop de
	ld (fpop+2),de
	jp (hl)

dofltop:
	call loadfregs
	push bc
	push ix
	ld hl,1f
	push hl
	push iy
	ret		! really a call
1:
	pop ix
	pop bc
	ld hl,(fpac+2)
	push hl
	ld hl,(fpac)
	jr phl

pop4:
	pop hl
	or h
	jr nz,9f
	ld a,l
	cp 4
	jr nz,9f
	jp (iy)
arg4:
	or d
	jr nz,9f
	ld a,(ix)
	inc ix
	cp 4
	jr nz,9f
	jp (iy)
9:	jr unimpld

adf.z:	ld iy,doadf
	jr pop4
adf.l:	ld d,(ix)
	inc ix
adf.s0:	ld iy,doadf
	jr arg4
doadf:
	ld iy,fpadd	! routine to call
	jr dofltop

sbf.z:	ld iy,dosbf
	jr pop4
sbf.l:	ld d,(ix)
	inc ix
sbf.s0:	ld iy,dosbf
	jr arg4
dosbf:
	ld iy,fpsub	! routine to call
	jr dofltop

mlf.z:	ld iy,domlf
	jr pop4
mlf.l:	ld d,(ix)
	inc ix
mlf.s0:	ld iy,domlf
	jr arg4
domlf:
	ld iy,fpmult	! routine to call
	jr dofltop

dvf.z:	ld iy,dodvf
	jr pop4
dvf.l:	ld d,(ix)
	inc ix
dvf.s0:	ld iy,dodvf
	jr arg4
dodvf:
	ld iy,fpdiv	! routine to call
	jr dofltop

cmf.z:	ld iy,docmf
	jr pop4
cmf.l:	ld d,(ix)
	inc ix
cmf.s0:	ld iy,docmf
	jr arg4
docmf:
	call loadfregs
	push bc
	push ix
	call fpcmf
	pop ix
	pop bc
	ld hl,(fpac)
	jr phl
cfi.z:
	pop de
	call chk24
	.data2 1f,0f
1:	ld iy,1f
	jr pop4
1:	pop hl
	ld (fpac),hl
	pop hl
	ld (fpac+2),hl
	push bc
	push ix
	call fpcfi
	pop ix
	pop bc
	ld hl,(fpac)
	jr phl
0:	ld iy,1f
	jr pop4
1:	pop hl
	ld (fpac),hl
	pop hl
ld (fpac+2),hl!
	push bc
	push ix
	call fpcfd
	jr 8f
cif.z:
	ld iy,1f
	jr pop4
1:
	pop de
	call chk24
	.data2 1f,0f
1:	pop hl
	ld (fpac),hl
	push bc
	push ix
	call fpcif
8:	pop ix
	pop bc
	ld hl,(fpac+2)
	push hl
	ld hl,(fpac)
	jr phl
0:	pop hl
	ld (fpac),hl
	pop hl
	ld (fpac+2),hl
	push bc
	push ix
	call fpcdf
	jr 8b

ngf.l:	ld d,(ix)
	inc ix
	ld iy,1f
	jr arg4
ngf.z:
	ld iy,1f
	jr pop4
1:	pop hl
	ld (fpac),hl
	pop hl
	ld (fpac+2),hl
	push bc
	push ix
	call fpcomp
	jr 8b

fif.z:
	ld iy,1f
	jr pop4
fif.l:
	ld d,(ix)
	inc ix
	ld iy,1f
	jr arg4
1:	call loadfregs
	push bc
	push ix
	call fpfif
	pop ix
	pop bc
	ld hl,(fpac+2)
	push hl
	ld hl,(fpac)
	push hl
	ld hl,(fpop+2)
	push hl
	ld hl,(fpop)
	jr phl

fef.z:
	ld iy,1f
	jr pop4
fef.l:
	ld d,(ix)
	inc ix
	ld iy,1f
	jr arg4
1:	pop hl
	ld (fpop),hl
	pop hl
	ld (fpop+2),hl
	push bc
	push ix
	call fpfef
	pop ix
	pop bc
	ld hl,(fpop+2)
	push hl
	ld hl,(fpop)
	push hl
	ld hl,(fpac)
	jr phl

! double aritmetic

adi.4:
	push bc
	pop iy
	pop hl
	pop de
	pop bc
	add hl,bc
	ex de,hl
	pop bc
	adc hl,bc
	push hl
	push de
	push iy
	pop bc
	jr loop
sbi.4:
	push bc
	pop iy
	pop bc
	pop de
	pop hl
	sbc hl,bc
	ex de,hl
	ld b,h
	ld c,l
	pop hl
9:
	sbc hl,bc
	push hl
	push de
	push iy
	pop bc
	jr loop
ngi.4:
	push bc
	pop iy
	ld hl,0
	pop de
	sbc hl,de
	ex de,hl
	ld hl,0
	pop bc
	jr 9b
mli.4:
	ld iy,.mli4
0:
	ld (retarea),bc
	ld (retarea+2),ix
	ld hl,1f
	push hl
	push iy
	ret
1:
	ld bc,(retarea)
	ld ix,(retarea+2)
	jr loop
dvu.4:
	ld iy,.dvu4
	jr 0b
	
dvi.4:
	ld iy,.dvi4
	jr 0b
	
! list of not yet implemented instructions
cuf.z:
cff.z:
cfu.z:
unimpld:		! used in dispatch table to
			! catch unimplemented instructions
	ld hl,EILLINS
9:	push hl
	jr trp.z

eilsize:
	ld hl,EILLSIZE
	jr 9b

e.case:
	ld hl,ECASE
	jr 9b
e.mon:
	ld hl,EBADMON
	jr 9b
e.array:
	push af
	ld a,(ignmask)
	bit 0,a
	jr nz,8f
	ld hl,EARRAY
	jr 9b
e.rck:
	push af
	ld a,(ignmask)
	bit 1,a
	jr nz,8f
	ld hl,ERANGE
	jr 9b
8:
	pop af
	ret

long2:	ld a,(ix)
	inc ix
	or a
	jr nz,unimpld
	ld a,(ix)
	inc ix
	cp 2
	jr nz,unimpld
	xor a		! clear carry
	ret

! monitor instruction
! a small collection of UNIX system calls implemented under CP/M

	ux_indir=e.mon
	ux_fork=e.mon
	ux_wait=e.mon
	ux_link=e.mon
	ux_exec=e.mon
	ux_chdir=e.mon
	ux_mknod=e.mon
	ux_chmod=e.mon
	ux_chown=e.mon
	ux_break=e.mon
	ux_stat=e.mon
	ux_seek=e.mon
	ux_mount=e.mon
	ux_umount=e.mon
	ux_setuid=e.mon
	ux_getuid=e.mon
	ux_stime=e.mon
	ux_ptrace=e.mon
	ux_alarm=e.mon
	ux_fstat=e.mon
	ux_pause=e.mon
	ux_utime=e.mon
	ux_stty=e.mon
	ux_gtty=e.mon
	ux_access=e.mon
	ux_nice=e.mon
	ux_sync=e.mon
	ux_kill=e.mon
	ux_dup=e.mon
	ux_pipe=e.mon
	ux_times=e.mon
	ux_prof=e.mon
	ux_unused=e.mon
	ux_setgid=e.mon
	ux_getgid=e.mon
	ux_sig=e.mon
	ux_umask=e.mon
	ux_chroot=e.mon

	EPERM	= 1
	ENOENT	= 2
	ESRCH	= 3
	EINTR	= 4
	EIO	= 5
	ENXIO	= 6
	E2BIG	= 7
	ENOEXEC	= 8
	EBADF	= 9
	ECHILD	= 10
	EAGAIN	= 11
	ENOMEM	= 12
	EACCES	= 13
	EFAULT	= 14
	ENOTBLK	= 15
	EBUSY	= 16
	EEXIST	= 17
	EXDEV	= 18
	ENODEV	= 19
	ENOTDIR	= 20
	EISDIR	= 21
	EINVAL	= 22
	ENFILE	= 23
	EMFILE	= 24
	ENOTTY	= 25
	ETXTBSY	= 26
	EFBIG	= 27
	ENOSPC	= 28
	ESPIPE	= 29
	EROFS	= 30
	EMLINK	= 31
	EPIPE	= 32
	EDOM	= 33
! Structure of filearea maintained by this implementation
! First iobuffer of 128 bytes
! Then the fcb area of 36 bytes
! The number of bytes left in the buffer, 1 byte
! The iopointer into the buffer, 2 bytes
! The openflag 0 unused, 1 reading, 2 writing, 1 byte
! The filedescriptor starting at 3, 1 byte
! The number of CTRL-Zs that have been absorbed, 1 byte
! The byte read after a sequence of CTRL-Zs, 1 byte

	maxfiles=8
	filesize=128+36+1+2+1+1+1+1

	filefcb=0	! pointers point to fcb
	position=33
	nleft=36
	iopointer=37
	openflag=39
	fildes=40
	zcount=41
	zsave=42

	.assert [ filefcb] == 0

0:	.space maxfiles*filesize
	filearea = 0b+128
sibuf:
	.data2 0
	.space 82
siptr:	.space 2
saveargs:
	.space 128
argv:	.space 40		! not more than 20 args
argc:	.space 2
ttymode:.data1 9,9,8,21;.data2 06310+RAW*040	! raw = 040

uxinit:
	xor a
	ld c,maxfiles
	ld hl,0b
1:	ld b,filesize
2:	ld (hl),a
	inc hl
	djnz 2b
	dec c
	jr nz,1b
	ret

uxfinish:
	ld a,maxfiles-1
1:	push af
	call closefil
	pop af
	dec a
	cp 0377
	jr nz,1b
	ret

makeargv:
	ld hl,0x80
	ld de,saveargs
	ld bc,128
	ldir
	ld hl,saveargs
	ld e,(hl)
	inc hl
	ld d,0
	add hl,de
	ld (hl),0
	ld hl,saveargs+1
	ld ix,argv
1:	ld a,(hl)
	or a
	jr z,9f
	cp ' '
	jr nz,2f
4:	ld (hl),0
	inc hl
	jr 1b
2:	ld (ix),l
	inc ix
	ld (ix),h
	inc ix
3:	inc hl
	ld a,(hl)
	or a
	jr z,9f
	cp ' '
	jr nz,3b
	jr 4b
9:	push ix
	pop hl
	ld de,argv
	or a
	sbc hl,de
	srl h;rr l
	ld (argc),hl
	ld (ix+0),0
	ld (ix+1),0
	ret

mon.z:
	pop de		! system call number
	xor a
	or d
	jr nz,unimpld	! too big
	ld a,e
	and 0300	! only 64 system calls
	jr nz,unimpld
	sla e
	ld hl,systab
	add hl,de
	ld e,(hl)
	inc hl
	ld d,(hl)
	ex de,hl
	jp (hl)

systab:	
	.data2 ux_indir
	.data2 ux_exit
	.data2 ux_fork
	.data2 ux_read
	.data2 ux_write
	.data2 ux_open
	.data2 ux_close
	.data2 ux_wait
	.data2 ux_creat
	.data2 ux_link
	.data2 ux_unlink
	.data2 ux_exec
	.data2 ux_chdir
	.data2 ux_time
	.data2 ux_mknod
	.data2 ux_chmod
	.data2 ux_chown
	.data2 ux_break
	.data2 ux_stat
	.data2 ux_seek
	.data2 ux_getpid
	.data2 ux_mount
	.data2 ux_umount
	.data2 ux_setuid
	.data2 ux_getuid
	.data2 ux_stime
	.data2 ux_ptrace
	.data2 ux_alarm
	.data2 ux_fstat
	.data2 ux_pause
	.data2 ux_utime
	.data2 ux_stty
	.data2 ux_gtty
	.data2 ux_access
	.data2 ux_nice
	.data2 ux_ftime
	.data2 ux_sync
	.data2 ux_kill
	.data2 unimpld
	.data2 unimpld
	.data2 unimpld
	.data2 ux_dup
	.data2 ux_pipe
	.data2 ux_times
	.data2 ux_prof
	.data2 ux_unused
	.data2 ux_setgid
	.data2 ux_getgid
	.data2 ux_sig
	.data2 unimpld
	.data2 unimpld
	.data2 unimpld
	.data2 unimpld
	.data2 unimpld
	.data2 ux_ioctl
	.data2 unimpld
	.data2 unimpld
	.data2 unimpld
	.data2 unimpld
	.data2 ux_exece
	.data2 ux_umask
	.data2 ux_chroot
	.data2 unimpld
	.data2 unimpld

emptyfile:
	! searches for a free filestructure
	! returns pointer in iy, 0 if not found
	ld iy,filearea
	ld l,maxfiles
1:
	xor a
	or (iy+openflag)
	jr nz,3f
	ld a,maxfiles+3
	sub l
	ld (iy+fildes),a
#ifdef	CPM1
	push bc
	push iy
	ld de,-128
	add iy,de
	push iy
	pop de
	ld c,setdma
	call bdos
	pop iy
	pop bc
	or a		! to clear C
#endif
	ret
3:
	ld de,filesize
	add iy,de
	dec l
	jr nz,1b
	scf
	ret

findfile:
	ld iy,filearea
	ld de,filesize
0:
	dec a
	ret m
	add iy,de
	jr 0b

getchar:
	push bc
	push de
	push hl
	dec (iy+nleft)
	jp p,0f
	push iy
	pop hl
	ld de,-128
	add hl,de
	ld (iy+iopointer),l
	ld (iy+iopointer+1),h
	ex de,hl
	push iy
	ld c,setdma
	call bdos
#ifdef	CPM1
	ld c,seqread
#else
	ld c,randomread
#endif
	pop de
	call bdos
	or a
	jr z,1f
	ld (iy+zcount),0
	pop hl
	pop de
	pop bc
	scf
	ret
1:
	inc (iy+position)
	jr nz,2f
	inc (iy+position+1)
2:
	ld a,127
	ld (iy+nleft),a
0:
	ld h,(iy+iopointer+1)
	ld l,(iy+iopointer)
	ld a,(hl)
	inc hl
	ld (iy+iopointer),l
	ld (iy+iopointer+1),h
	pop hl
	pop de
	pop bc
	ret
	or a

putchar:
	push hl
	ld h,(iy+iopointer+1)
	ld l,(iy+iopointer)
	ld (hl),a
	dec (iy+nleft)
	jr z,0f
	inc hl
	ld (iy+iopointer+1),h
	ld (iy+iopointer),l
	pop hl
	ret
0:
	pop hl
flsbuf:
	push hl
	push de
	push bc
	push iy
	pop hl
	ld de,-128
	add hl,de
	ld (iy+iopointer+1),h
	ld (iy+iopointer),l
	ex de,hl
	push iy
	ld c,setdma
	call bdos
	pop de
#ifdef	CPM1
	ld c,seqwrite
#else
	ld c,randomwrite
#endif
	call bdos
	or a
	jr z,1f
	pop bc
	pop de
	pop hl
	scf
	ret
1:
	inc (iy+position)
	jr nz,2f
	inc (iy+position+1)
2:
	ld a,128
	ld (iy+nleft),a
	ld b,a
	push iy
	pop hl
	ld de,-128
	add hl,de
	ld a,26			! ctrl z
1:	ld (hl),a
	inc hl
	djnz 1b
	pop bc
	pop de
	pop hl
	or a
	ret

parsename:
	! parses file name pointed to by hl and fills in fcb
	! of the file pointed to by iy.
	! recognizes filenames as complicated as 'b:file.zot'
	! and as simple as 'x'

	push bc
	push iy
	pop de
	xor a
	push de
	ld b,36		! sizeof fcb
0:	ld (de),a
	inc de
	djnz 0b
	pop de
	inc hl
	ld a,(hl)
	dec hl
	cp ':'		! drive specified ?
	jr nz,1f
	ld a,(hl)
	inc hl
	inc hl
	dec a
	and 15
	inc a		! now 1<= a <= 16
	ld (de),a
1:	inc de
	ld b,8		! filename maximum of 8 characters
1:	ld a,(hl)
	or a
	jr nz,8f
	dec hl
	ld a,'.'
8:
	inc hl
	cp '.'
	jr z,2f
	and 0177	! no parity
	bit 6,a
	jr z,9f
	and 0337	! UPPER case
9:
	ld (de),a
	inc de
	djnz 1b
	ld a,(hl)
	inc hl
	cp '.'
	jr z,3f
	ld a,' '
	ld (de),a
	inc de
	ld (de),a
	inc de
	ld (de),a
	pop bc
	ret		! filenames longer than 8 are truncated
2:	ld a,' '	! fill with spaces
0:	ld (de),a
	inc de
	djnz 0b
3:	ld b,3		! length of extension
1:	ld a,(hl)
	inc hl
	or a
	jr z,4f
	cp 0100
	jp m,2f
	and 0137
2:	ld (de),a
	inc de
	djnz 1b
	pop bc
	ret
4:	ld a,' '
0:	ld (de),a
	inc de
	djnz 0b
	pop bc
	ret

! various routines
ux_close:
	pop hl
	ld a,l
	sub 3
	jp m,1f
	cp maxfiles
	call m,closefil
1:	ld hl,0
	jr phl

closefil:
	call findfile
	ld a,(iy+openflag)
	or a
	jr z,3f
	ld (iy+openflag),0
	cp 1
	jr z,2f
	ld a,(iy+nleft)
	cp 128
	jr z,2f
	call flsbuf
2:
	push bc
	push iy
	pop de
	ld c,close
	call bdos
	pop bc
3:	ret

ux_ioctl:
	pop hl
	ld a,l
	sub 3
	jp p,1f
	pop hl
	ld a,h
	cp 't'
	jr nz,e.mon
	ld a,l
	cp 8
	jr z,tiocgetp
	cp 9
	jr z,tiocsetp
	jr e.mon
1:	pop hl
	pop hl
	ld hl,-1
	jr phl
tiocgetp:
	pop de
	ld hl,ttymode
2:	push bc
	ld bc,6
	ldir
	ld h,b
	ld l,c
	pop bc
	jr phl
tiocsetp:
	pop hl
	ld de,ttymode
	jr 2b

ux_time:
	call time4
	jr loop

ux_ftime:
	pop hl
	ld (retarea+6),hl
	call time4
	ld hl,(retarea+6)
	pop de
	ld (hl),e
	inc hl
	ld (hl),d
	inc hl
	pop de
	ld (hl),e
	inc hl
	ld (hl),d
	inc hl
	xor a
	ld (hl),a
	inc hl
	ld (hl),a
	inc hl
	ld (hl),a
	inc hl
	ld (hl),a
	inc hl
	ld (hl),a
	inc hl
	ld (hl),a
	jr loop

time4:
	pop hl
	ld (retarea),bc
	ld (retarea+2),ix
	ld (retarea+4),hl
	ld hl,(timebuf+2)
	push hl
	ld hl,(timebuf)
	push hl
	ld hl,0
	push hl
	ld hl,50
	push hl
	call .dvu4
	ld bc,(retarea)
	ld ix,(retarea+2)
	ld hl,(retarea+4)
	jp (hl)
ux_exit:
	call uxfinish
	ld c,reset
	call bdos
	! no return

ux_creat:
	call emptyfile
	jr c,openfailed
	pop hl
	call parsename
	pop hl			! file mode, not used under CP/M
	push bc
	push iy
	push iy
	pop de
	ld c,delete
	call bdos
	pop de
	ld c,makefile
	call bdos
	pop bc
	ld l,1
	jr afteropen
ux_open:
	call emptyfile
	jr nc,1f
openfailed:
	pop hl
	pop hl		! remove params
	ld hl,EMFILE
	push hl
	jr phl
1:
	pop hl		! filename
	call parsename
	push bc
	ld c,open
	push iy
	pop de
	call bdos
	pop bc
	pop hl
afteropen:
	inc a
	jr nz,1f
	ld hl,ENOENT
	push hl
	jr phl
1:
	inc l
	ld (iy+openflag),l
	xor a
	ld (iy+nleft),a
	ld (iy+zcount),a
	ld (iy+zsave),26
	bit 1,l
	jr z,2f
	ld (iy+nleft),128
2:
	ld (iy+position),a
	ld (iy+position+1),a
	push iy
	pop hl
	push bc
	ld b,128
3:	dec hl
	ld (hl),26
	djnz 3b
	pop bc
	ld (iy+iopointer+1),h
	ld (iy+iopointer),l
	ld h,a
	ld l,(iy+fildes)
	push hl
	ld l,a
	jr phl

ux_read:
	pop hl
	ld a,l
	sub 3
	jp p,readfile
	ld a,(ttymode+4)
	bit 5,a
	jr z,1f			! not raw
	push bc
#ifdef	CPM1
!raw echo interface
	ld c,consolein
	call bdos
#else
!no echo interface
4:
	ld c,diconio
	ld e,0xff
	call bdos
	or a
	jr z,4b
!end of no echo interface
#endif
	pop bc
	pop hl
	ld (hl),a
	pop hl
	ld hl,1
	push hl
	ld hl,0
	jr phl
1:
	ld hl,sibuf+1		! read from console assumed
	dec (hl)
	jp p,2f
	dec hl			! go read console line
	ld (hl),80		! max line length
	push bc
	push hl
	ld c,readconsole
	ex de,hl
	call bdos
	ld c,writeconsole
	ld e,'\n'
	call bdos
	pop hl
	pop bc
	inc hl
	inc (hl)
	ld (siptr),hl		! ready for transfer
	push hl
	ld e,(hl)
	ld d,0
	add hl,de
	ld (hl),'\r'
	inc hl
	ld (hl),'\n'
	pop hl
2:
	push bc
	pop iy
	ld b,(hl)
	inc b			! bytes remaining
	pop hl			! copy to
	pop de			! bytes wanted (probably 512)
	push iy
	ld iy,(siptr)		! copy from
	xor a			! find out minimum of ramaining and wanted
	or d
	jr nz,3f		! more than 255 wanted (forget that)
	ld a,b
	cp e
	jp m,3f			! not enough remaining
	ld b,e
3:
	ld c,b			! keep copy
0:
	inc iy
	ld a,(iy)
	ld (hl),a
	inc hl
	djnz 0b
	ld a,(sibuf+1)
	sub c
	inc a
	ld (sibuf+1),a
	ld (siptr),iy
	pop hl
	push bc
	ld c,b
	push bc			! load 0
	ld b,h
	ld c,l
	jr loop
readfile:
	call findfile
	pop de
	pop hl			! count
	push bc
	ld bc,0
0:
	xor a
	or l
	jr z,1f
	dec l
3:
! warning: this may not work if zcount overflows
	ld a,(iy+zcount)
	or a
	jr nz,5f
	ld a,(iy+zsave)
	cp 26
	jr z,4f
	ld (iy+zsave),26
	jr 8f
4:
	call getchar
	jr c,2f
	ld (de),a
	sub 26		! CTRL-Z
	jr z,7f
	ld a,(iy+zcount)
	or a
	jr z,6f
	ld a,(de)
	ld (iy+zsave),a
5:
	ld a,26
	dec (iy+zcount)
8:
	ld (de),a
6:
	inc de
	inc bc
	jr 0b
1:
	dec l
	dec h
	jp p,3b
2:
	pop hl
	push bc
	ld b,h
	ld c,l
	ld hl,0
	jr phl
7:
	inc (iy+zcount)
	jr 4b

ux_write:
	pop hl
	ld a,l
	sub 3
	jp p,writefile
	pop hl			! buffer address
	pop de			! count
	push de
	ld iy,0
	push iy
	push bc
	ld b,e			! count now in 'db'
0:
	ld a,b
	or a
	jr nz,1f
	ld a,d
	or a
	jr nz,2f
	pop bc
	jr loop
2:
	dec d
1:
	dec b
	ld e,(hl)
	inc hl
	push bc
	push de
	push hl
	ld c,writeconsole
	call bdos
	pop hl
	pop de
	pop bc
	jr 0b
writefile:
	call findfile
	pop de
	pop hl			! count
	push bc
	ld bc,0
0:
	xor a
	or l
	jr z,1f
	dec l
3:
	ld a,(de)
	inc de
	call putchar
	jr c,4f
	inc bc
	jr 0b
1:
	dec l
	dec h
	jp p,3b
	ld iy,0
2:
	pop hl
	push bc
	ld b,h
	ld c,l
	push iy
	jr loop
4:
	ld iy,ENOSPC
	jr 2b

ux_unlink:
	pop hl
	ld iy,fcb
	call parsename
	push bc
	ld c,delete
	ld de,fcb
	call bdos
	pop bc
	inc a
	jr nz,1f
	ld hl,ENOENT
	jr phl
1:
	ld hl,0
	jr phl

ux_getpid:
	ld hl,12345		! nice number
	jr phl

ux_exece:
	ld iy,fcb
	pop hl
	call parsename
	pop hl
	ld b,h;ld c,l
	pop iy
	ld ix,0x82
	ld (ix-1),' '
4:	ld h,b;ld l,c
3:	ld e,(hl)
	inc hl
	ld d,(hl)
	inc hl
	ld b,h;ld c,l
	ex de,hl
	ld a,h
	or l
	jr z,1f
2:
	ld a,(hl)
	inc hl
	ld (ix),a
	inc ix
	or a
	jr nz,2b
	ld (ix-1),' '
	jr 4b
1:
	ld (ix),'X'
	ld (ix+1),'\r'
	ld (ix+2),'\n'
	ld (ix+3),'$'
	ld de,0x81
	push ix
	ld c,printstring
	call bdos
	pop hl
	ld de,-129
	add hl,de
	ld a,l
	ld (0x80),a
	jr warmstart




dispat1:	! base for escaped opcodes
.data2	aar.l,	aar.z,	adf.l,	adf.z,	adi.l,	adi.z,	ads.l,	ads.z
.data2	adu.l,	adu.z,	and.l,	and.z,	asp.l,	ass.l,	ass.z,	bge.l
.data2	bgt.l,	ble.l,	blm.l,	bls.l,	bls.z,	blt.l,	bne.l,	cai.z
.data2	cal.l,	cfi.z,	cfu.z,	ciu.z,	cmf.l,	cmf.z,	cmi.l,	cmi.z
.data2	cms.l,	cms.z,	cmu.l,	cmu.z,	com.l,	com.z,	csa.l,	csa.z
.data2	csb.l,	csb.z,	cuf.z,	cui.z,	cuu.z,	dee.l,	del.p,	del.n
.data2	dup.l,	dus.l,	dus.z,	dvf.l,	dvf.z,	dvi.l,	dvi.z,	dvu.l
.data2	dvu.z,	fef.l,	fef.z,	fif.l,	fif.z,	inl.p,	inl.n,	inn.l
.data2	inn.z,	ior.l,	ior.z,	lar.l,	lar.z,	ldc.l,	ldf.l,	ldl.p
.data2	ldl.n,	lfr.l,	lil.p,	lil.n,	lim.z,	los.l,	los.z,	lor.s0
.data2	lpi.l,	lxa.l,	lxl.l,	mlf.l,	mlf.z,	mli.l,	mli.z,	mlu.l
.data2	mlu.z,	mon.z,	ngf.l,	ngf.z,	ngi.l,	ngi.z,	nop.z,	rck.l
.data2	rck.z,	ret.l,	rmi.l,	rmi.z,	rmu.l,	rmu.z,	rol.l,	rol.z
.data2	ror.l,	ror.z,	rtt.z,	sar.l,	sar.z,	sbf.l,	sbf.z,	sbi.l
.data2	sbi.z,	sbs.l,	sbs.z,	sbu.l,	sbu.z,	sde.l,	sdf.l,	sdl.p
.data2	sdl.n,	set.l,	set.z,	sig.z,	sil.p,	sil.n,	sim.z,	sli.l
.data2	sli.z,	slu.l,	slu.z,	sri.l,	sri.z,	sru.l,	sru.z,	sti.l
.data2	sts.l,	sts.z,	str.s0,	tge.z,	tle.z,	trp.z,	xor.l,	xor.z
.data2	zer.l,	zer.z,	zge.l,	zgt.l,	zle.l,	zlt.l,	zne.l,	zrf.l
.data2	zrf.z,	zrl.p,	dch.z,	exg.s0,	exg.l,	exg.z,	lpb.z

dispat2:	! base for 4 byte offsets
.data2	ldc.f


ignmask: .data2 0	! ignore mask (variable)
retarea: .data2 0	! base of buffer for result values (max 8 bytes)
	 .data2 0
	 .data2 0
	 .data2 0

trapproc:
	.data2 0

nextp:	.data1 0

header:
ntext:	.data2 0
ndata:	.data2 0
nproc:	.data2 0
entry:	.data2 0
nline:	.data2 0

hp:	.data2 0
pb:	.data2 0
pd:	.data2 0
