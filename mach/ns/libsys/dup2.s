.define _dup2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _dup2
.sect .text
_dup2:
enter[], 0
movd 8(fp),r7
ord 64,r7
movd 12(fp),tos
movd r7,tos
movd 41,tos
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
