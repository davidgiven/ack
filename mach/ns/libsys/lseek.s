.define _lseek
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _lseek
.sect .text
_lseek:
enter[], 0
addr 8(fp),r7
movd 16,r0
addd r0,r7
1:
subd 4,r7
movd 0(r7),tos
acbd -4,r0,1b
movd 19,tos
jsr @.mon
cmpqd 0,tos
beq I0011
movd tos,r7
movd r7,@_errno
movd -1,tos
I0011:
movd tos,r4
movd tos,r5
exit []
ret 0
