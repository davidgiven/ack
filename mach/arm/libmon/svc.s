.define _EmMon, _EmTrp
 
.sect .text
 
_EmMon:
 LDR R11,[R12]
 CMP R11,#1
 BNE _2
!
! EXIT
!
 LDR R2,[R12,#4]
 MOV R1,#0x41
 ADD R1,R1,#0x4200
 ADD R1,R1,#0x450000
 ADD R1,R1,#0x58000000
 SWI 17  ! os_exit
_2:
 CMP R11,#3
 BNE _3
!
! READ
!
 MOV R0,#4
 LDR R1,[R12,#4]  !fildes
 CMP R1,#5
 BGE _22
!
! READ FROM THE KEYBOARD
!
 LDR R0,[R12,#8]  !buffer
 MOV R10,R0
 LDR R1,[R12,#12]  ! char count
 CMP R1,#256
 MOV.CS R1,#255   !OS_Readline max
 MOV R2,#0
 MOV R3,#126
 SWI 14  !OS_READLINE
! CR TO LF
 LDR.B R0,[R10,R1]
 CMP R0,#13
 BNE _23
 MOV R0,#10
 STR.B R0,[R10,R1]
_23:
 ADD R1,R1,#1 ! INC BUFFER COUNT
 STMFD R12<,{R1}
 MOV R0,#0
 STMFD R12<,{R0}
 MOV R15,R14
_22:
! READ FROM A FILE
 LDR R2,[R12,#8]
 LDR R3,[R12,#12]
 STR R5,_savreg
 STR R4,_savreg2
 MOV R5,R3
 SWI 12  !os_gbpb
 SUB R5,R5,R3
 STMFD R12<,{R5}
 MOV R5,#0
 STMFD R12<,{R5}
 LDR R5,_savreg
 LDR R4,_savreg2
 MOV R15,R14
_3:
 CMP R11,#4
 BNE _4
!
! WRITE
!
 LDR R1,[R12,#4]
 CMP R1,#5
 BGE _33
! WRITE TO THE SCREEN
 LDR R0,[R12,#8]
 LDR R1,[R12,#12]
 SWI 70  !os_writeN
! APPEND CR TO LINE ENDING WITH LF
 LDR R0,[R12,#12]
 SUB R0,R0,#1
 LDR R1,[R12,#8]
 LDR.B R2,[R1,R0]
 CMP R2,#10
 SWI.EQ 269  !OUTPUT CR
 LDR R1,[R12,#12]
 STMFD R12<,{R1}
 MOV R1,#0
 STMFD R12<,{R1}
 MOV R15,R14
_33:
! WRITE TO A FILE
 MOV R0,#2
 LDR R2,[R12,#8]
 LDR R3,[R12,#12]
 STR R5,_savreg
 STR R4,_savreg2
 SWI 12  !os_gbpb
 LDR R1,[R12,#12]   !always success
 STMFD R12<,{R1}
 LDR R5,_savreg
 LDR R4,_savreg2
 MOV R0,#0
 STMFD R12<,{R0}
 MOV R15,R14
_4:
 CMP R11,#5
 BNE _5
!
! OPEN
!
 LDR R2,[R12,#8]
 CMP R2,#0
 BNE _46
 MOV R0,#67  !READ ONLY
 BAL _45
_46:
 CMP R2,#2
 BNE _44
 MOV R0,#195  !UPDATE, FILE MUST EXIST
 BAL _45
_44:
 MOV R0,#131  !CREATE AND OPEN FOR UPDATE
_45:
 LDR R1,[R12,#4]  !FILE NAME
 MOV R2,#0  !NO PATH
 SWI 0x2000D  !Xos_find
 MOV.VS R0,#0
 CMP R0,#0 !MAP ERROR CODES
 MOV.EQ R0,#-1
 STMFD R12<,{R0}
 MOV R2,#0
 STMFD R12<,{R2}
 MOV R15,R14
_5:
 CMP R11,#6
 BNE _6
!
! CLOSE
!
 MOV R0,#0
 LDR R1,[R12,#4]
 CMP R1,#5
 BLT _51  !IGNORE IF NOT FILE
 SWI 0x2000D  !Xos_find (IGNORE ERRORS)
_51:
 MOV R0,#0
! STMFD R12<,{R0}  !not sure...
 STMFD R12<,{R0}
 MOV R15,R14
_6:
 CMP R11,#8
 BNE _7
!
! CREAT
!
 BAL _44
_7:
 CMP R11,#10
 BNE _8
!
! UNLINK
!
! note that on UNIX, unlink may be
! done on an open file. This would
! cause an error on RiscOs, which is ignored
 MOV R0,#6
 LDR R1,[R12,#4]
 STR R5,_savreg
 STR R4,_savreg2
 CMP R1,#5
 SWI.GE 0x20008  !os_file
 LDR R5,_savreg
 LDR R4,_savreg2
 MOV R8,#0
! STMFD R12<,{R8}  ! not sure...
 STMFD R12<,{R8}
 MOV R15,R14
_8:
 CMP R11,#12
 BNE _9
!
! CHDIR
!
 MOV R0,#0
 LDR R1,[R12,#4]
 SWI 41  !os_fscontrol
 MOV R0,#0
 STMFD R12<,{R0}  !not sure...
 STMFD R12<,{R0}
 MOV R15,R14
_9:
 CMP R11,#19
 BNE _10
!
! LSEEK
!
 MOV R0,#0  !FIRST GET CURRENT PTR
 LDR R1,[R12,#4]
 SWI 9  !OS_ARGS
 MOV R11,R2
 LDR R3,[R12,#12]  !NOW DO THE LSEEK
 CMP R3,#2
 BEQ _92
 CMP R3,#1
 BEQ _91
 LDR R1,[R12,#4]
 LDR R2,[R12,#8]
_90:
 MOV R0,#1
 SWI 9  !os_args
 BAL _99
_92:
 MOV R0,#2
 LDR R1,[R12,#4]
 SWI 9  !os_args
 LDR R8,[R12,#8]
 ADD R2,R2,R8
 LDR R1,[R12,#4]
 BAL _90
_91:
 MOV R0,#0
 LDR R1,[R12,#4]
 SWI 9  !os_args
 LDR R3,[R12,#8]
 ADD R2,R1,R3
 BAL _90
_99:
 STMFD R12<,{R11}
 MOV R0,#0
 STMFD R12<,{R0}
 MOV R15,R14
_10:
 CMP R11, #54
 BNE _1001
!
! IOCTL
! always returns 0
!
 MOV R0, #0
 STMFD R12<, {R0}
 MOV R15, R14
!
! TIMES
!
_1001:
 CMP R11, #43
 BNE _1003
 ADR R1,_17
 MOV R0,#3
 STR R0,[R1]
 MOV R0,#14
 SWI 7  !OS_WORD
 ADR R0,_17
 LDR R0,[R0]
 MOV R1,#0
 LDR R2,[R12,#4]
 STR R0,[R2]
 STR R0,[R2,#4] ! utime = stime
 STR R1,[R2,#8]
 STR R1,[R2,#12]
 STMFD R12<,{R1}
 MOV R15,R14
!
! GETPID
! returns random number (clock ticks)
!
_1003:
 CMP R11, #20
 BNE _1002
 ADR R1,_17
 MOV R0,#3
 STR R0,[R1]
 MOV R0,#14
 SWI 7  !OS_WORD
 ADR R0,_17
 LDR R0,[R0]
 AND R0,R0,#0xFF
 STMFD R12<,{R0}
 MOV R15,R14
!
! EXECE
! ONLY FIRST PARAMETER IS USED
!
_1002:
 CMP R11, #59
 BNE _1000
 LDR R0,[R12,#4]
 SWI 5  !OS_CLI
 MOV R0,#0
 STMFD R12<,{R0}
 MOV R15,R14
! 
! NOT IMPLEMENTED CALLS
! ---------------------
! THESE CALLS MAY REQUIRE DIFFERENT
! STACK HANDLING (THERE MAY BE BUGS HERE!!)
! (the number of return values does not always
! correspond to the Ack manual)
!
_1000:
 CMP R11,#7
 BNE _9726
 ADR R0,_9607
 BAL _9352
_9726:
 CMP R11,#26
 BNE _9727
 ADR R0,_9626
 BAL _9352
_9727:
 CMP R11,#27
 BNE _9741
 ADR R0,_9627
 BAL _9352
_9741:
 CMP R11,#41
 BNE _9742
 ADR R0,_9641
 BAL _9352
_9742:
 CMP R11,#42
 BNE _9748
 ADR R0,_9642
 BAL _9352
_9748:
 CMP R11,#48
 BNE _9760
 ADR R0,_9648
 BAL _9352
_9760:
 CMP R11,#60
 BNE _9709
 ADR R0,_9660
 BAL _9352
_9709:
 CMP R11,#9
 BNE _9714
 ADR R0,_9609
 BAL _9351
_9714:
 CMP R11,#14
 BNE _9715
 ADR R0,_9614
 BAL _9351
_9715:
 CMP R11,#15
 BNE _9716
 ADR R0,_9615
 BAL _9351
_9716:
 CMP R11,#16
 BNE _9718
 ADR R0,_9616
 BAL _9351
_9718:
 CMP R11,#18
 BNE _9720
 ADR R0,_9618
 BAL _9351
_9720:
 CMP R11,#20
 BNE _9721
 ADR R0,_9620
 BAL _9351
_9721:
 CMP R11,#21
 BNE _9722
 ADR R0,_9621
 BAL _9351
_9722:
 CMP R11,#22
 BNE _9723
 ADR R0,_9622
 BAL _9351
_9723:
 CMP R11,#23
 BNE _9724
 ADR R0,_9623
 BAL _9351
_9724:
 CMP R11,#24
 BNE _9725
 ADR R0,_9624
 BAL _9351
_9725:
 CMP R11,#25
 BNE _9728
 ADR R0,_9625
 BAL _9351
_9728:
 CMP R11,#28
 BNE _9729
 ADR R0,_9628
 BAL _9351
_9729:
 CMP R11,#29
 BNE _9730
 ADR R0,_9629
 BAL _9351
_9730:
 CMP R11,#30
 BNE _9733
 ADR R0,_9630
 BAL _9351
_9733:
 CMP R11,#33
 BNE _9734
 ADR R0,_9633
 BAL _9351
_9734:
 CMP R11,#34
 BNE _9735
 ADR R0,_9634
 BAL _9351
_9735:
 CMP R11,#35
 BNE _9736
 ADR R0,_9635
 BAL _9351
_9736:
 CMP R11,#36
 BNE _9737
 ADR R0,_9636
 BAL _9351
_9737:
 CMP R11,#37
 BNE _9744
 ADR R0,_9637
 BAL _9351
_9744:
 CMP R11,#44
 BNE 9746
 ADR R0,_9644
 BAL _9351
_9746:
 CMP R11,#46
 BNE 9747
 ADR R0,_9646
 BAL _9351
_9747:
 CMP R11,#47
 BNE 9751
 ADR R0,_9647
 BAL _9351
_9751:
 CMP R11,#51
 BNE 9753
 ADR R0,_9651
 BAL _9351
_9753:
 CMP R11,#53
 BNE 9756
 ADR R0,_9653
 BAL _9351
_9756:
 CMP R11,#56
 BNE 9761
 ADR R0,_9656
 BAL _9351
_9761:
 ADR R0,_9661
 BAL _9351

 
.sect .data
_14:
.data4 0
.data4 0
_15:
.asciz "Monitor call "
.data4 0
.align 4
_16:
.asciz ": not implemented"
.data4 0
.align 4
_17:
.data4 3
.data4 0
.align 4

.sect .text

_EmTrp:
 SWI 3  !OS_NEWLINE
 LDR R11,[R12]
!
! deal with ignore mask here
! (not yet implemented)
!
 CMP R11,#0
 BNE _9101
 ADR R0,_9000
 BAL _9200
_9101:
 CMP R11,#1
 BNE _9102
 ADR R0,_9001
 BAL _9200
_9102:
 CMP R11,#2
 BNE _9103
 ADR R0,_9002
 BAL _9200
_9103:
 CMP R11,#3
 BNE _9104
 ADR R0,_9003
 BAL _9200
_9104:
 CMP R11,#4
 BNE _9105
 ADR R0,_9004
 BAL _9200
_9105:
 CMP R11,#5
 BNE _9106
 ADR R0,_9005
 BAL _9200
_9106:
 CMP R11,#6
 BNE _9107
 ADR R0,_9006
 BAL _9200
_9107:
 CMP R11,#7
 BNE _9108
 ADR R0,_9007
 BAL _9200
_9108:
 CMP R11,#8
 BNE _9109
 ADR R0,_9008
 BAL _9200
_9109:
 CMP R11,#9
 BNE _9110
 ADR R0,_9009
 BAL _9200
_9110:
 CMP R11,#10
 BNE _9111
 ADR R0,_9010
 BAL _9200
_9111:
 CMP R11,#11
 BNE _9112
 ADR R0,_9011
 BAL _9200
_9112:
 CMP R11,#12
 BNE _9113
 ADR R0,_9012
 BAL _9200
_9113:
 CMP R11,#13
 BNE _9114
 ADR R0,_9013
 BAL _9200
_9114:
 CMP R11,#14
 BNE _9115
 ADR R0,_9014
 BAL _9200
_9115:
 CMP R11,#15
 BNE _9116
 ADR R0,_9015
 BAL _9200
_9116:
 CMP R11,#16
 BNE _9117
 ADR R0,_9016
 BAL _9200
_9117:
 CMP R11,#17
 BNE _9118
 ADR R0,_9017
 BAL _9200
_9118:
 CMP R11,#18
 BNE _9119
 ADR R0,_9018
 BAL _9200
_9119:
 CMP R11,#19
 BNE _9120
 ADR R0,_9019
 BAL _9200
_9120:
 CMP R11,#20
 BNE _9121
 ADR R0,_9020
 BAL _9200
_9121:
 CMP R11,#21
 BNE _9122
 ADR R0,_9021
 BAL _9200
_9122:
 CMP R11,#22
 BNE _9123
 ADR R0,_9022
 BAL _9200
_9123:
 CMP R11,#23
 BNE _9124
 ADR R0,_9023
 BAL _9200
_9124:
 CMP R11,#24
 BNE _9125
 ADR R0,_9024
 BAL _9200
_9125:
 CMP R11,#25
 BNE _9126
 ADR R0,_9025
 BAL _9200
_9126:
 CMP R11,#26
 BNE _9127
 ADR R0,_9026
 BAL _9200
_9127:
 CMP R11,#27
 BNE _9128
 ADR R0,_9027
 BAL _9200
_9128:
 CMP R11,#127
 BGT _9129
 ADR R0,_9028
 SWI 2  !OS_WRITE0
 MOV R0,R11
 ADR R1,_12
 MOV R2,#5
 SWI 217  !CONVERT
 SUB R1,R1,R0
 SWI 70  !WRITEN
 BAL _9300
_9129:
 ADR R0,_11
 SWI 2  !OS_WRITE0
 MOV R0,R11
 ADR R1,_12
 MOV R2,#5
 SWI 217  !CONVERT
 SUB R1,R1,R0
 SWI 70  !WRITEN
 BAL _9300
_9200: SWI 2
_9300:
 ADR R0, _Lineno
 LDR R0,[R0]
 CMP R0,#0
 BEQ _9301
 MOV R10,R0
 ADR R0,_9050
 SWI 2
 MOV R0,R10
 ADR R1,_12
 MOV R2,#5
 SWI 217
 SUB R1,R1,R0
 SWI 70
 SWI 3
 BAL _9302
_9301:
 ADR R0,_9501
 SWI 2
 SWI 3
_9302:
 MOV R0,#1
 STMFD R12<,{R0}
 STMFD R12<,{R0}
 BAL.L _EmMon
_9351:
 SWI 2  !WRITE0
 ADR R0,_16
 SWI 2 !OS_WRITE0
 SWI 3 !NEWLINE
 MOV R0, #0
 STMFD R12<,{R0}
 MOV R15,R14
_9352:
 SWI 2  !WRITE0
 ADR R0,_16
 SWI 2 !OS_WRITE0
 SWI 3 !NEWLINE
 MOV R0, #0
 STMFD R12<,{R0}
 STMFD R12<,{R0}
 MOV R15,R14


.sect.data
_11:
.asciz "Trap "
.align 4
_12:
.data4 0
.data4 0
_9000:
.asciz "Array bound error "
.align 4
_9001:
.asciz "Range bound error "
.align 4
_9002:
.asciz "Set bound error "
.align 4
_9003:
.asciz "Integer overflow "
.align 4
_9004:
.asciz "Floating overflow "
.align 4
_9005:
.asciz "Floating underflow "
.align 4
_9006:
.asciz "Divide by 0 "
.align 4
_9007:
.asciz "Divide by 0.0 "
.align 4
_9008:
.asciz "Undefined integer "
.align 4
_9009:
.asciz "Undefined float "
.align 4
_9010:
.asciz "Conversion error "
.align 4
_9011:
.asciz "unknown trap "
.align 4
_9012:
.asciz "unknown trap "
.align 4
_9013:
.asciz "unknown trap "
.align 4
_9014:
.asciz "unknown trap "
.align 4
_9015:
.asciz "unknown trap "
.align 4
_9016:
.asciz "Stack overflow "
.align 4
_9017:
.asciz "Heap overflow "
.align 4
_9018:
.asciz "Illegal instruction "
.align 4
_9019:
.asciz "Illegal size argument "
.align 4
_9020:
.asciz "Case error "
.align 4
_9021:
.asciz "Addressing non-existent memory "
.align 4
_9022:
.asciz "Bad pointer used "
.align 4
_9023:
.asciz "Program counter out of range "
.align 4
_9024:
.asciz "Bad argument of LAE "
.align 4
_9025:
.asciz "Bad monitor call "
.align 4
_9026:
.asciz "Argument of LIN too high "
.align 4
_9027:
.asciz "GOTO decriptor error "
.align 4
_9028:
.asciz "Language run-time trap "
.align 4
_9501:
.asciz " (line number unknown)"
.align 4
_9050:
.asciz " at line "
.align 4

_9607:
.asciz "Wait"
.align 4
_9609:
.asciz "Link"
.align 4
_9614:
.asciz "Mknod"
.align 4
_9615:
.asciz "Chmod"
.align 4
_9616:
.asciz "Chown"
.align 4
_9618:
.asciz "Stat"
.align 4
_9620:
.asciz "Getpid"
.align 4
_9621:
.asciz "Mount"
.align 4
_9622:
.asciz "Umount"
.align 4
_9623:
.asciz "Setuid"
.align 4
_9624:
.asciz "Getuid"
.align 4
_9625:
.asciz "Stime"
.align 4
_9626:
.asciz "Ptrace"
.align 4
_9627:
.asciz "Alarm"
.align 4
_9628:
.asciz "Fstat"
.align 4
_9629:
.asciz "Pause"
.align 4
_9630:
.asciz "Utime"
.align 4
_9633:
.asciz "Access"
.align 4
_9634:
.asciz "Nice"
.align 4
_9635:
.asciz "Ftime"
.align 4
_9636:
.asciz "Sync"
.align 4
_9637:
.asciz "Kill"
.align 4
_9641:
.asciz "Dup"
.align 4
_9642:
.asciz "Pipe"
.align 4
_9644:
.asciz "Profil"
.align 4
_9646:
.asciz "Setgid"
.align 4
_9647:
.asciz "Getgid"
.align 4
_9648:
.asciz "Sigtrp"
.align 4
_9651:
.asciz "Acct"
.align 4
_9653:
.asciz "Lock"
.align 4
_9656:
.asciz "Mpxcall"
.align 4
_9660:
.asciz "Umask"
.align 4
_9661:
.asciz "Chroot"
.align 4

.sect .data
_savreg:
.data4 0
_savreg2:
.data4 0


