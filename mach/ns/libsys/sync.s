.define _sync
.extern _sync
.text
_sync:
enter[], 0
movd 36,tos
jsr @.mon
exit []
ret 0
