.define  EXIT, F_DUM
.define  ERANGE, ESET, EHEAP, EILLINS, EODDZ, ECASE, EBADMON
.define  hol0, trppc, trpim, reghp, argv, envp

EXIT	= 0
F_DUM	= 0

ERANGE  = 1
ESET    = 2
EHEAP   = 17
EILLINS	= 18
EODDZ	= 19
ECASE   = 20
EBADMON = 25

.text
			!clear .bss
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
	calr	_m_a_i_n
	ldl	RR14, $0xC00017FC
	sc	$0

.bss
begbss:
.data
hol0:
	.word 0,0			! line no
	.word 0,0			! file
trppc:
	.word 0
trpim:
	.word 0
argv:
envp:
	.word 1f
	.word 0
1:	.asciz	"program"
reghp:
	.word endbss
