.define __setsig
.bss
_save:.space	4
_trf:.space	4
.extern __setsig
.text
__setsig:
enter[], 0
movd 8(fp),@_trf
movd @.trpreg,r0
movd _catchit,@.trpreg
movd r0,@_save
exit []
ret 0
_catchit:
enter[], 0
movd @.trpreg,r0
movd _catchit,@.trpreg
movd r0,tos
adjspd -4
movd @_trf,r7
movd 8(fp),tos
jsr r7
cmpqd 0,r4
bne I0021
movd @.trpreg,r0
movd @_save,@.trpreg
movd r0,tos
adjspd -4
jsr @.trp
movd @.trpreg,r0
movd _catchit,@.trpreg
movd r0,tos
adjspd -4
movd tos,r7
movd r7,@_save
I0021:
exit []
ret 0
