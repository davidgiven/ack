! head_em.s
.define begdat,begbss,hol0,argv,envp,_RegHp,_incptflag
.define _Lineno,_Filna,_IgnoreMask,_TrpReg

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.base 0x8000
 ADR R0,begbss
 ADR R1,endbss
 MOV R2,#0
 ADR R3,_incptflag
 STR R2,[R3]
1:
 STR R2,[R0],#4
 CMP R0,R1
 BLT 1B
! the next 2 lines are OS specific:
 SWI 16			! OS_Getenv
 SUB R12, R1, #8	! Get end of ram
! R0 points to command string
! (argv,argc) code for C
! or Pascal file names
 MOV R9,R0  ! save pt in R0
 MOV R8,#0  ! string length
len:
 LDR.B R7,[R9],#1
 CMP R7,#0
 ADD.NE R8,R8,#1
 BNE len
 MOV R7,R8  ! save len in R7
! round up to multiple of 4
 ADD R8,R8,#4
 MVN R6, #3   !FFFFFFFC
 AND R8,R8,R6
! make room on stack
 SUB R12,R12,R8  ! word aligned
! now determine argc
! OS intercepts blank lines, so
! there must be at least one arg
 MOV R9,R0
 MOV R6,#0  ! holds argc
 MOV R5,#0  ! char count
loop1:
 CMP R5,R7
 BGT next1
 LDR.B R4,[R9],#1
 ADD R5,R5,#1
 CMP R4,#32
 BEQ loop1  !skip spaces
 ADD R6,R6,#1   ! new arg
nospace1:
 CMP R5,R7
 BGT next1
 LDR.B R4,[R9],#1
 ADD R5,R5,#1
 CMP R4,#32
 BNE nospace1
 CMP R5,R7
 BCC loop1
! BEQ loop1
next1:
! store argc
 ADR R5,argc
 STR R6,[R5]
! copy the string
! onto the stack
 MOV R9,R0  !source
 MOV R2,R12 !dest
 MOV R3,R7  !#chars
loop2:
 LDR.B R4,[R9],#1
 STR.B R4,[R2],#1
 SUB.S R3,R3,#1
 BNE loop2
 MOV R4, #0  ! zero terminator
 STR.B R4,[R2]
! create space on the stack for
! the pointers to individual args
 MOV R11, R12  ! string pt
 MOV R8,R6,LSL #2
 SUB R12,R12,R8
! save pointer to argv[0]
 ADR R1,argv
 STR R12,[R1]
! determine argv[0] .. argv[argc-1]
! and store on stack
! R11 = start of string
! R7  = string lenth
! R12 = stack pointer
! R6  = argc
 MOV R9, #0  !char count
 MOV R8, #0  !arg count
 MOV R5,R11
loop3:
 CMP R9,R7
 BGT next2
 LDR.B R4,[R5],#1
 ADD R9,R9,#1
 CMP R4,#32
 BEQ loop3
 CMP R9,R7
 BGT next2
 SUB R4,R5,#1 !compensate for incr
 STR R4,[R12,R8,LSL #2]
 ADD R8,R8,#1
nospace2:
 CMP R9,R7
 BGT next2
 LDR.B R4,[R5],#1
 ADD R9,R9,#1
 CMP R4,#32
 BNE nospace2
 CMP R9,R7
 BCC loop3
! BEQ loop3
! finally, replace spaces by 0
next2:
 MOV R5,R11  !string ptr
 MOV R6,R7   !string length
 MOV R3,#0
loop4:
 CMP R6,#0
 BEQ next3
 LDR.B R4,[R5]
 CMP R4,#32
 STR.EQ.B R3,[R5]
 ADD R5,R5,#1
 SUB R6,R6,#1
 BAL loop4
next3:
 ADR R11, envp
 STMFD R12<,{R11}
 ADR R11,argv
 LDR R11,[R11]
 STMFD R12<,{R11}
 ADR R11, argc
 LDR R11,[R11]
 STMFD R12<,{R11}
 BAL.L __m_a_i_n
 MOV R11,#0
 STMFD R12<,{R11}
 MOV R11,#1
 STMFD R12<,{R11}
 BAL.L _EmMon

.sect .bss
begbss:
.sect .data
begdat:
hol0:
 .data4 0
 .data4 0
argv:
 .data4 0
envp:
 .data4 0
argc:
.data4 0
2:
 .asciz 'PROGRAM'
 .align
_RegHp:
 .data4 endbss
_Lineno:
 .data4	0
_Filna:
 .data4 0
_IgnoreMask:
 .data4 0
_TrpReg:
 .data4 0
_incptflag:
.data4 0

