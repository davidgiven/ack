#
! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

.define .trp
.define EARRAY, ERANGE, ESET, EIOVFL, EFOVFL, EFUNFL, EIDIVZ, EIDIVZ
.define EFDIVZ, EIUND, EFUND, ECONV, ESTACK, EHEAP, EILLINS, EODDZ
.define ECASE, EMEMFLT, EBADPTR, EBADPC, EBADLAE, EBADMON, EBADLIN, EBADGTO
.define EUNIMPL

! Trap routine
! Expects trap number in A, and must be called directly from the code
! where execution should resume (for those traps which support it).
! Just returns if trap has to be ignored.
! Otherwise it calls a user-defined trap handler if provided.
! When no user-defined trap handler is provided or when the user-defined
! trap handler causes a new trap, a message is printed
! and control is returned to the monitor.

	EARRAY	=  0
	ERANGE	=  1
	ESET	=  2
	EIOVFL	=  3
	EFOVFL	=  4
	EFUNFL	=  5
	EIDIVZ	=  6
	EFDIVZ	=  7
	EIUND	=  8
	EFUND	=  9
	ECONV	= 10
	ESTACK  = 16
	EHEAP	= 17
	EILLINS = 18
	EODDZ	= 19
	ECASE	= 20
	EMEMFLT	= 21
	EBADPTR = 22
	EBADPC  = 23
	EBADLAE = 24
	EBADMON = 25
	EBADLIN = 26
	EBADGTO = 27
	EUNIMPL = 63		! unimplemented em-instruction called

.trp:
	cp 16
	jr nc, 3f			! jump if trap cannot be ignored

! check if trap has to be ignored
	push hl
	push bc

	ld hl, (.ignmask)
	ld b, a
	inc b
shift_loop:
	rrc h
	rr l
	djnz shift_loop		! loop shifts one extra time

	pop bc
	pop hl

	jr nc, 3f			! jump if trap should not be ignored
	and 1

	! Trap is being ignored. We've restored the stack, so just return.
2:
	ret

3:
.define .trapproc
.trapproc = . + 1
	ld hl, 0			! user defined trap handler held inline here
	ld de, 0
	and a
	sbc hl, de
	jr z, _panic		! if zero, panic

	ld (.trapproc), de	! reset user defined trap handler
	ld e, a
	push de				! push trap number onto stack

	ld de, 2b
	push de
	jp (hl)				! call track handler

