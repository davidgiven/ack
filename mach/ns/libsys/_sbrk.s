.define __sbrk
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __sbrk
.sect .text
__sbrk:
enter[], 0
movd @.reghp,r7
addd 8(fp),r7
movd r7,@.reghp
movd @.reghp,r4
exit []
ret 0
