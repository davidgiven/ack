.define __brk
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __brk
.sect .text
__brk:
enter[], 0
movd 8(fp),@.reghp
movd 0,r4
exit []
ret 0
