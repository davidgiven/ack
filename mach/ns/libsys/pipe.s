.define _pipe
.extern _pipe
.text
_pipe:
enter[], 0
movd 42,tos
jsr @.mon
cmpqd 0,tos
beq I0011
movd tos,r7
movd r7,@_errno
movd -1,r4
exit []
ret 0
I0011:
movd 8(fp),r7
movd tos,r6
movd r6,4(r7)
movd tos,r6
movd r6,0(8(fp))
movd 0,r4
exit []
ret 0
