.define __exit
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __exit
.sect .text
__exit:
enter[], 0
movd 8(fp),tos
movd 1,tos
jsr @.mon
