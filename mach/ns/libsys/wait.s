.define _wait
.extern _wait
.text
_wait:
enter[], 0
movd 7,tos
jsr @.mon
cmpqd 0,tos
bne I0011
cmpqd 0,8(fp)
beq I0012
movd tos,r7
movd r7,0(8(fp))
movd tos,r6
movd r6,r4
exit []
ret 0
I0012:
adjspd -4
movd tos,r7
movd r7,r4
exit []
ret 0
I0011:
movd tos,r7
movd r7,@_errno
movd -1,r4
exit []
ret 0
