.define __close
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __close
.sect .text
__close:
movd 0, r4
ret 0
