.define __fork
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__fork:
enter[], 0
movd 2,tos
jsr @.mon
cmpqd 0,tos
beq I0011
movd tos,r7
movd r7,@_errno
movd -1,r4
exit []
ret 0
I0011:
cmpqd 0,tos
beq I0012
movd tos,r7
movd 0,tos
I0012:
movd tos,r7
movd r7,r4
exit []
ret 0
