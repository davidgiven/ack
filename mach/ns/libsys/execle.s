.define _execle
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _execle
.sect .text
_execle:
enter[], 0
addr 12(fp),tos
I0011:
movd tos,r7
movd r7,tos
cmpd 0(r7),0
beq I0012
movd tos,r7
addr 4(r7),tos
br I0011
I0012:
movd tos,r7
movd 4(r7),tos
addr 12(fp),tos
movd 8(fp),tos
movd 59,tos
jsr @.mon
movd tos,r7
movd r7,@_errno
movd -1,r4
exit []
ret 0
