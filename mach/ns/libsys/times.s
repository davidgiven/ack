.define _times
.extern _times
.text
_times:
enter[], 0
movd 8(fp),tos
movd 43,tos
jsr @.mon
exit []
ret 0
