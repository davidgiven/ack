.define _open
.extern _open
.text
_open:
enter[], 0
movd 12(fp),tos
movd 8(fp),tos
movd 5,tos
jsr @.mon
cmpqd 0,tos
beq I0011
movd tos,r7
movd r7,@_errno
movd -1,tos
I0011:
movd tos,r7
movd r7,r4
exit []
ret 0
