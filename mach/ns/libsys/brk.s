.define _brk
.extern _brk
.text
_brk:
enter[], 0
movd 8(fp),@.reghp
movd 0,r4
exit []
ret 0
