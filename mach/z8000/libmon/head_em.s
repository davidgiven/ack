.define  EXIT, WRITE, F_DUM
.define  ERANGE, ESET, EHEAP, EILLINS, EODDZ, ECASE, EBADMON
.define  hol0, trppc, trpim, reghp, argv, envp
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

F_DUM	= 0

ERANGE  = 1
ESET    = 2
EHEAP   = 17
EILLINS	= 18
EODDZ	= 19
ECASE   = 20
EBADMON = 25

.sect .text
			!clear .sect .bss
	ldk	R2, $0
	ld	R3, $endbss
	ld	R0, R3
	sub	R0, $begbss
	jr	EQ, 1f
	sra	R0
	push	*RR2, $0
	dec	R0
	jr	EQ, 1f
	ldl	RR4, RR2
	dec	R5, $2
	lddr	*RR4, *RR2, R0
1:
	ldb	RL0, $10		! echo newline
	sc	$4
	ldl	RR14, $0
	push	*RR14, envp
	push	*RR14, argv
	push	*RR14, $1
	calr	__m_a_i_n
EXIT:
	ldl	RR14, $0xC00017FC
	sc	$0

WRITE:	jr	__write

.sect .bss
begbss:
.sect .data
hol0:
	.data2 0,0			! line no
	.data2 0,0			! file
trppc:
	.data2 0
trpim:
	.data2 0
argv:
envp:
	.data2 1f
	.data2 0
1:	.asciz	"program"
reghp:
	.data2 endbss
