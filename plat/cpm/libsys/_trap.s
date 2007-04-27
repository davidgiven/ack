#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.define .trp
.define earray, erange, eset, eiovfl, efovfl, efunfl, eidivz, eidivz
.define efdivz, eiund, efund, econv, estack, eheap, eillins, eoddz
.define ecase, ememflt, ebadptr, ebadpc, ebadlae, ebadmon, ebadlin, ebadgto
.define eunimpl

.sect .text

! Trap routine
! Expects trap number on stack.
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

earray:	lxi h,EARRAY
	push h
	call .trp
	ret

erange:	lxi h,ERANGE
	push h
	call .trp
	ret

eset:	lxi h,ESET
	push h
	call .trp
	ret

eiovfl:	lxi h,EIOVFL
	push h
	call .trp
	ret

efovfl:	lxi h,EFOVFL
	push h
	call .trp
	ret

efunfl:	lxi h,EFUNFL
	push h
	call .trp
	ret

eidivz:	lxi h,EIDIVZ
	push h
	call .trp
	ret

efdivz:	lxi h,EFDIVZ
	push h
	call .trp
	ret

eiund:	lxi h,EIUND
	push h
	call .trp
	ret

efund:	lxi h,EFUND
	push h
	call .trp
	ret

econv:	lxi h,ECONV
	push h
	call .trp
	ret

estack:	lxi h,ESTACK
	push h
	call .trp
	ret

eheap:	lxi h,EHEAP
	push h
	call .trp
	ret

eillins:lxi h,EILLINS
	push h
	call .trp
	ret

eoddz:	lxi h,EODDZ
	push h
	call .trp
	ret

ecase:	lxi h,ECASE
	push h
	call .trp
	ret

ememflt:lxi h,EMEMFLT
	push h
	call .trp
	ret

ebadptr:lxi h,EBADPTR
	push h
	call .trp
	ret

ebadpc:	lxi h,EBADPC
	push h
	call .trp
	ret

ebadlae:lxi h,EBADLAE
	push h
	call .trp
	ret

ebadmon:lxi h,EBADMON
	push h
	call .trp
	ret

ebadlin:lxi h,EBADLIN
	push h
	call .trp
	ret

ebadgto:lxi h,EBADGTO
	push h
	call .trp
	ret

eunimpl:lxi h,EUNIMPL
	push h
	call .trp
	ret

.trp:
	pop h
	xthl
	push h			! trap number and return address exchanged
	mov a,l
	cpi 16
	jnc 3f			! jump if trap cannot be ignored

! check if trap has to be ignored
	xchg			! de = trap number
	lhld .ignmask
	push h			! hl = set to be tested
	push d
	call .inn2		! de = 1 if bit is set, 0 otherwise
	mov a,e
	rar
	jnc 3f			! jump if trap should not be ignored
	pop h			! remove trap number
	ret			! OGEN DICHT EN ... SPRING!!!

3:
	lhld .trapproc		! user defined trap handler?
	mov a,l
	ora h
	jz 1f			! jump if there was not
	xra a
	sta .trapproc		! .trapproc := 0
	sta .trapproc+1
	lxi d,2f		
	push d
	pchl			! call user defined trap handler
2:
	pop d
	ret
1:
	lxi h, 1
	push h
	lxi h, text
	push h
	lxi h, 6
	push h
	call _write
	jmp EXIT

.sect .rom
text: .ascii "TRAP!\n"

