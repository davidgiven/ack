.define _signal
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.sect .data
_vector:
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
.data4	0
_mapvec:
.data4	0
.data4	134219784
.data4	2048
.data4	0
.data4	2827
.data4	658176
.data4	3072
firsttim:
.extern _signal
.data4	1
.sect .text
_signal:
enter[], 16
movd 8(fp),r7
addd -1,r7
movd r7,-4(fp)
cmpqd 0,-4(fp)
bgt I0014
cmpd -4(fp),16
blt I0013
I0014:
movd -1,tos
br I0011
I0013:
cmpqd 0,@firsttim
beq I0015
xord @firsttim,@firsttim
movd catchtrp,tos
jsr @__setsig
adjspd -4
I0015:
movd -4(fp),r7
ashd 2,r7
movd _vector(r7),-12(fp)
cmpd -12(fp),12(fp)
beq I0016
movd -4(fp),r7
ashd 2,r7
movd 12(fp),_vector(r7)
cmpd 12(fp),1
bne I0017
movd -3,-16(fp)
br I0018
I0017:
cmpqd 0,12(fp)
bne I0019
movd -2,-16(fp)
br I0018
I0019:
addr 128(8(fp)),-16(fp)
I0018:
movd 8(fp),tos
movd -16(fp),tos
jsr @_sigtrp
adjspd -8
cmpd r4,-1
bne I0016
movd -1,tos
br I0011
I0016:
movd -12(fp),tos
I0011:
movd tos,r7
movd r7,r4
exit []
ret 0
catchtrp:
enter[], 0
cmpd 8(fp),128
ble I0023
cmpd 8(fp),144
bgt I0023
movd 8(fp),r7
subd 128,r7
movd r7,tos
jsr @procesig
adjspd -4
movd r4,tos
br I0021
I0023:
cmpqd 0,8(fp)
bgt I0024
cmpd 8(fp),28
bge I0024
movd _mapvec,r7
addd 8(fp),r7
movzbd 0(r7),r6
cmpqd 0,r6
beq I0024
movd _mapvec,r7
addd 8(fp),r7
movzbd 0(r7),tos
jsr @procesig
adjspd -4
movd r4,tos
br I0021
I0024:
movd 0,tos
I0021:
movd tos,r7
movd r7,r4
exit []
ret 0
procesig:
enter[], 8
movd 8(fp),r7
addd -1,r7
movd r7,-4(fp)
movd -4(fp),r6
ashd 2,r6
movd _vector(r6),-8(fp)
cmpd -8(fp),1
bne I0033
movd 1,tos
br I0031
I0033:
cmpd 8(fp),4
beq I0034
cmpd 8(fp),5
beq I0034
movd -4(fp),r7
ashd 2,r7
movd 1,_vector(r7)
I0034:
cmpqd 0,-8(fp)
bne I0035
movd 0,tos
br I0031
I0035:
movd -8(fp),r7
movd 8(fp),tos
jsr r7
adjspd -4
movd 1,tos
I0031:
movd tos,r7
movd r7,r4
exit []
ret 0
