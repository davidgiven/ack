.define __exit
.extern __exit
.text
__exit:
enter[], 0
movd 8(fp),tos
movd 1,tos
jsr @.mon
