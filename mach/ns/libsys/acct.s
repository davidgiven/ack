.define _acct
.extern _acct
.text
_acct:
enter[], 0
movd 8(fp),tos
movd 51,tos
jsr @.mon
cmpqd 0,tos
bne I0011
movd 0,r4
exit []
ret 0
I0011:
movd tos,r7
movd r7,@_errno
movd -1,r4
exit []
ret 0
