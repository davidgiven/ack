.define __getpid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __getpid
.sect .text
__getpid:
enter[], 0
movd 20,tos
jsr @.mon
movd tos,r7
movd r7,r4
exit []
ret 0
