.define _umask
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _umask
.sect .text
_umask:
enter[], 0
movd 8(fp),tos
movd 60,tos
jsr @.mon
movd tos,r7
movd r7,r4
exit []
ret 0
