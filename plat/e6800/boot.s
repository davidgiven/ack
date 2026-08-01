.define BASE, EM_BSIZE
.define NBYTES
.define hol0, IGNMASK, ADDR, PROGNAME
.define LB, LBl, ERRPROC
.define	ARTH, RETURN, SIGN
.define RETSIZE, TRAPVAL, BRANCH
.define START
.define TMP, TMP2
.define	__exit, EXIT
.define	_putchar, _getchar, _print, _cpu_counter, _errno
.define _putstr, _puthexl,_puthexi, _puthexc
.define _emu6800_conout, _emu6800_conin

BASE    = 240
EM_BSIZE = 4

.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .end


.sect .zero
hol0:   .space 16       ! the hol0 block
IGNMASK: .space 2	! can hold the ingnore mask
ADDR: .space 4          ! used for indirect addressing
LB: .space 2            ! the localbase
LBl: .space 2           ! the second localbase (localbase-BASE)
ERRPROC: .space 2	! can hold the address of the error handler
ARTH: .space 16         ! used for arithmetic
RETURN: .space 4        ! the return area
SIGN: .space 1
RETSIZE: .space 1
TRAPVAL: .space 1
BRANCH: .space 2
NBYTES: .space 2
TMP: .space 2
TMP2: .space 2
exitsp: .space 2

_errno: .space 2

.define Earray, Erange, Eset
.define Eiovfl, Eidivz, Eiund, Econv
.define Estack, Eheap, Eillins, Eoddz
.define Ecase , Ebadmon
.define Ebadlin, Ebadgto
Earray  = 0
Erange  = 1
Eset    = 2
Eiovfl  = 3
Eidivz  = 6
Eiund   = 8
Econv   = 10
Estack  = 16
Eheap   = 17
Eillins = 18
Eoddz   = 19 
Ecase   = 20
Ebadmon = 25
Ebadlin = 26
Ebadgto = 27

.define F_DUM
F_DUM	= 0		! Dummy floating point constant

!.base 0x0100            ! where to start in the emu6800
.sect .text
! GENERAL PURPOSE ROUTINES
START:
	sts	exitsp
	lds	#0xefff
!
	clrb
	clra
	pshb		! envp
	psha
	pshb		! argv
	psha
	pshb		! argc
	psha
	jsr     __m_a_i_n	! see.  lang/cem/libcc/gen/head_cc.e
	ins
	ins
	ins
	ins
___exit:
EXIT:
	lds exitsp
	staa 0xfefb
	ldab RETURN+1
	stab 0xfeff
	rts
__exit:
	tsx
	ldab 3,x
	stab RETURN+1
	bra ___exit
!
!	minimal I/O routine
!
_puthexc:
	tsx
	bra	_puthexi3
_puthexl:
	tsx
	bsr	_puthexi2
	inx
	inx
	bra	_puthexi2
_puthexi:
	tsx
_puthexi2:
	ldaa	2,x
	bsr	_puthex2
_puthexi3:
	ldaa	3,x
_puthex2:			! put AccA in 2 hexdigit
	bsr	_puthexhi
	tab
	bra	_puthexlo
_puthexhi:
	tab
	lsrb
	lsrb
	lsrb
	lsrb
_puthexlo:
	andb	#0x0f
	cmpb	#0x0a
	bcs	1f
	addb	#'A'-'9'-1
1:	addb	#'0'
	stab	0xfefe
	rts
_putstr:
	stab	<TMP+1
	staa	<TMP
	ldx	<TMP
	bra	2f
1:	stab	0xfefe
	inx
2:	ldab	0,x
	bne	1b
	rts
_emu6800_conout:
_putchar:
	tsx
	ldab	3,x
	stab	0xfefe
	rts
_emu6800_conin:
_getchar:
	ldab	0xfefe
	stab	RETURN+1
	clr	RETURN
	rts
_print:
	tsx
	ldab	3,x
	ldaa	2,x
	staa	0xfefc
	stab	0xfefd
	rts
_cpu_counter:
	staa	0xfefb
	rts
.sect .data
.define _emu6800_ram
_emu6800_ram: .data2 __end
PROGNAME:               ! for initialising the programname pointer
.asciz "emu6800"
.sect .bss
beginbss:
