.define _execve
.extern _execve
.text
_execve:
enter[], 0
addr 8(fp),r7
movd 12,r0
addd r0,r7
1:
subd 4,r7
movd 0(r7),tos
acbd -4,r0,1b
movd 59,tos
jsr @.mon
movd tos,r7
movd r7,@_errno
movd -1,r4
exit []
ret 0
