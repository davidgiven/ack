.define EARRAY,ERANGE,EILLINS,EILLSIZE,ECASE,EMON,EHEAP
.define hol0,trapproc,trpim,argv,hp,.reghp,envp,begbss,ignmask
.define savebc,savede,savehl,saveix,saveaf,saveiy,ebadmon
   EARRAY = 0
   ERANGE = 1
   EHEAP = 17
   EILLINS=18
   EILLSIZE=19
   ECASE=20
   EMON=25
   ebadmon=25


	.base 0x1000
.text

				! clear .bss
	ld sp,0x7ffe		!address of fbase
	ld de,endbss
	ld h,d
	ld l,e
	ld bc,begbss
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
	call _m_a_i_n

	jp 0x20

.bss
begbss:
.data
hol0:
	.word 0,0
	.word 0,0
saveaf:
	.word 0
savebc:
	.word 0
savede:
	.word 0
savehl:
	.word 0
saveix:
	.word 0
saveiy:
	.word 0
ignmask:
	.word 0
hp:
	.word 0
trapproc:
	.word 0
trpim:
	.word 0
argv:
	.word 3f
envp:
	.word 0
3:
	.asciz 'PROGRAM'
.reghp:
	.word endbss
