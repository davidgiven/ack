.define _nice
.extern _nice
.text
_nice:
enter[], 0
movd 8(fp),tos
movd 34,tos
jsr @.mon
exit []
ret 0
