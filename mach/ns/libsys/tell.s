.define _tell
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _tell
.sect .text
_tell:
enter[], 0
movd 1,tos
movd 0,tos
movd 8(fp),tos
jsr @_lseek
adjspd -12
exit []
ret 0
