#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $_absf
 pro $_absf,0
 mes 5
 mes 9,8
 lal 0
 loi EM_FSIZE
 zrf EM_FSIZE
 cmf EM_FSIZE
 zlt *3
 lal 0
 loi EM_FSIZE
 bra *4
3
 lal 0
 loi EM_FSIZE
 ngf EM_FSIZE
4
 ret EM_FSIZE
 end 0
