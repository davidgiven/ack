.define __cleanup
.extern __cleanup
.text
__cleanup:
tst.b -40(sp)
link	a6,#-0
unlk a6
rts
