.define _mknod
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _mknod
.sect .text
_mknod:
enter[], 0
addr 8(fp),r7
movd 12,r0
addd r0,r7
1:
subd 4,r7
movd 0(r7),tos
acbd -4,r0,1b
movd 14,tos
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
