.define _getpid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _getpid
.sect .text
_getpid:
enter[], 0
movd 20,tos
jsr @.mon
movd tos,r7
movd r7,r4
exit []
ret 0
