.define _exit
.extern _exit
.text
_exit:
enter[], 0
jsr @__cleanup
movd 8(fp),tos
jsr @__exit
adjspd -4
exit []
ret 0
