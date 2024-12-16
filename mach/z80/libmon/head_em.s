.define EARRAY,ERANGE,EILLINS,EILLSIZE,ECASE,EBADMON,EHEAP
.define hol0,trapproc,trpim,argv,hp,.reghp,envp,begbss,ignmask
.define savebc,savede,savehl,saveix,saveaf,saveiy,ebadmon
.define WRITE,EXIT
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
   EARRAY = 0
   ERANGE = 1
   EHEAP = 17
   EILLINS=18
   EILLSIZE=19
   ECASE=20
   EBADMON=25
   ebadmon=25


.sect .text

				! clear .sect .bss
	ld sp,0x7ffe		!address of fbase
	ld de,endbss
	ld h,d
	ld l,e
	ld bc,begbss
	xor a			! to clear carry
	sbc hl,bc
	ld a,h
	or l
	jr z,1f
2:
	xor a
	ld (de),a
	dec de
	dec hl
	ld a,h
	or l
	jr nz,2b
1:				! hl == 0

	ld bc,envp
	push bc
	ld bc,argv
	push bc
	ld bc,1
	push bc
	call __m_a_i_n
EXIT:
	jp 0x20
WRITE:
	jp __write

.sect .bss
begbss:
.sect .data
hol0:
	.data2 0,0
	.data2 0,0
saveaf:
	.data2 0
savebc:
	.data2 0
savede:
	.data2 0
savehl:
	.data2 0
saveix:
	.data2 0
saveiy:
	.data2 0
ignmask:
	.data2 0
hp:
	.data2 0
trapproc:
	.data2 0
trpim:
	.data2 0
argv:
	.data2 3f
envp:
	.data2 0
3:
	.asciz 'PROGRAM'
.reghp:
	.data2 endbss
