.define _ftime
.extern _ftime
.text
_ftime:
enter[], 0
movd 8(fp),tos
movd 35,tos
jsr @.mon
exit []
ret 0
