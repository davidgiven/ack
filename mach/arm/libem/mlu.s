.define .mlu

.sect .text

.mlu:
 LDMFD R12<,{R0,R1}
 MOV R2, #0
_mluloop:
 MOV.S R0,R0,LSR #1
 ADD.CS R2,R2,R1
 ADD R1,R1,R1
 BNE _mluloop
 MOV R0,R2
 MOV R15,R14
