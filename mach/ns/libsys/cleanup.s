.define __cleanup
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __cleanup
.sect .text
__cleanup:
enter[], 0
exit []
ret 0
