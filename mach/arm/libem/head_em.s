.define 	begdat, begbss
.define	hol0, argv, envp, reghp
.define ignmask, trpreg, rcktrap

.sect	.text
.sect	.rom
.sect	.data
.sect	.bss

.sect 	.text

	ADR R0,begbss
	ADR R1,_endbss
	MOV R2,#0
1:
	STR R2,[R0],#4
	CMP R0,R1
	BGT 1b

	MOV R12, #0x100000
	ADR R11, envp
	STMFD R12<, {R11}
	ADR R11, argv
	STMFD R12<, {R11}
	MOV R11, #1
	STMFD R12<, {R11}
	BAL.L _m_a_i_n
rcktrap:
	SWI 1

.sect .bss
begbss:
.sect .data
begdat:
hol0:
	.data4 0
	.data4 0
argv:
	.data4 2f
envp:
	.data4 0
2:
	.asciz 'PROGRAM'
reghp:	
	.data4 _endbss
ignmask:
	.data4 0
trpreg:
	.data4 0
