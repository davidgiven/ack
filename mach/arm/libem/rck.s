.define _RckTrap,_CasErr
.sect .text

_RckTrap:
 MOV R0,#1
 STMFD R12<,{R0}
 BAL.L _EmTrp
 
_CasErr:
 MOV R0,#20
 STMFD R12<,{R0}
 BAL.L _EmTrp
 
 
