.define _alarm
.extern _alarm
.text
_alarm:
enter[], 0
movd 8(fp),tos
movd 27,tos
jsr @.mon
movd tos,r7
movd r7,r4
exit []
ret 0
