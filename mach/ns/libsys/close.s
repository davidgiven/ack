.define _close
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _close
.sect .text
_close:
movd 0, r4
ret 0
